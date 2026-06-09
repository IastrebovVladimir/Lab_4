#ifndef MACHINE_RUNNER_H
#define MACHINE_RUNNER_H

#include <cstddef>
#include <stdexcept>
#include <string>
#include "StateMachine.h"
#include "TurnstileStateMachine.h"
#include "../Stream/ReadOnlyStream.h"
#include "../Stream/WriteOnlyStream.h"
#include "../FileStream/FileReadOnlyStream.h"
#include "../FileStream/FileWriteOnlyStream.h"

template <typename State, typename Event, typename Result>
class MachineRunner {
private:
    struct StateHistory {
        State states[20];
        size_t countOfBuffer = 0;
    };

    StateMachine<State, Event, Result>& machine;
    ReadOnlyStream<Event>& input;
    WriteOnlyStream<Result>& output;
    StateHistory history;
    size_t countOfAllStates = 0;

    static std::string SerializeStateForLog(const State& state) {
        if (state == TurnstileState::Locked) {
            return "Locked";
        }
        if (state == TurnstileState::Unlocked) {
            return "Unlocked";
        }
        throw std::runtime_error("Unknown state");
    }

    static State ParseStateFromLog(const std::string& line) {
        if (line == "Locked") {
            return TurnstileState::Locked;
        }
        if (line == "Unlocked") {
            return TurnstileState::Unlocked;
        }
        throw std::runtime_error("Unknown state in log file");
    }

public:
    MachineRunner(StateMachine<State, Event, Result>& machine,
                  ReadOnlyStream<Event>& input,
                  WriteOnlyStream<Result>& output)
            : machine(machine), input(input), output(output) {}

    void Run() {
        FileWriteOnlyStream<State> stateLog("state_log.txt", SerializeStateForLog);

        input.Open();
        output.Open();
        stateLog.Open();

        try {
            while (true) {
                Event event;

                try {
                    event = input.Read();
                } catch (const std::runtime_error& error) {
                    if (std::string(error.what()) == "End of stream") {
                        break;
                    }
                    throw;
                }

                Result result = machine.Step(event);
                output.Write(result);

                State currentState = machine.GetCurrentState();
                stateLog.Write(currentState);

                history.states[countOfAllStates % 20] = currentState;
                if (history.countOfBuffer < 20) {
                    history.countOfBuffer++;
                }
                countOfAllStates++;
            }
        } catch (...) {
            stateLog.Close();
            output.Close();
            input.Close();
            throw;
        }

        stateLog.Close();
        output.Close();
        input.Close();
    }

    const State& GetState(size_t index) const {
        if (history.countOfBuffer == 0) {
            throw std::out_of_range("History index is out of range");
        }

        size_t firstValidIndex = countOfAllStates - history.countOfBuffer;
        size_t lastValidIndex = countOfAllStates - 1;

        if (index < firstValidIndex || index > lastValidIndex) {
            throw std::out_of_range("History index is out of range");
        }

        return history.states[index % 20];
    }

    State GetStateLog(size_t index) const {
        FileReadOnlyStream<State> stateLog("state_log.txt", ParseStateFromLog);
        stateLog.Open();

        try {
            size_t currentIndex = 0;
            while (true) {
                State state = stateLog.Read();
                if (currentIndex == index) {
                    stateLog.Close();
                    return state;
                }
                currentIndex++;
            }
        } catch (const std::runtime_error& error) {
            stateLog.Close();
            if (std::string(error.what()) == "End of stream") {
                throw std::out_of_range("State log index is out of range");
            }
            throw;
        }
    }

    size_t GetStateCount() const {
        return countOfAllStates;
    }
};

#endif