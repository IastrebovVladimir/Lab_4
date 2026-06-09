#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

template <typename State, typename Event, typename Result>
class StateMachine {
protected:
    State currentState;

public:
    explicit StateMachine(const State& initialState)
            : currentState(initialState) {}

    virtual ~StateMachine() = default;

    const State& GetCurrentState() const {
        return currentState;
    }

    virtual Result Step(const Event& event) = 0;
};

#endif