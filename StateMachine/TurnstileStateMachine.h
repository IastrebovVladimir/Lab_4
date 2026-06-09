#ifndef TURNSTILE_STATE_MACHINE_H
#define TURNSTILE_STATE_MACHINE_H

#include <string>
#include "StateMachine.h"

enum class TurnstileState {
    Locked,
    Unlocked
};

enum class TurnstileEvent {
    Coin,
    Push
};

struct TurnstileTransitionResult {
    TurnstileState oldState;
    TurnstileEvent event;
    TurnstileState newState;
    std::string action;
};

class TurnstileStateMachine : public StateMachine<TurnstileState, TurnstileEvent, TurnstileTransitionResult> {
public:
    TurnstileStateMachine()
            : StateMachine<TurnstileState, TurnstileEvent, TurnstileTransitionResult>(TurnstileState::Locked) {}

    explicit TurnstileStateMachine(TurnstileState initialState)
            : StateMachine<TurnstileState, TurnstileEvent, TurnstileTransitionResult>(initialState) {}

    TurnstileTransitionResult Step(const TurnstileEvent& event) override {
        TurnstileTransitionResult result;
        result.oldState = currentState;
        result.event = event;

        if (currentState == TurnstileState::Locked) {
            if (event == TurnstileEvent::Coin) {
                currentState = TurnstileState::Unlocked;
                result.action = "Unlock";
            } else {
                currentState = TurnstileState::Locked;
                result.action = "Alarm";
            }
        } else {
            if (event == TurnstileEvent::Push) {
                currentState = TurnstileState::Locked;
                result.action = "Lock";
            } else {
                currentState = TurnstileState::Unlocked;
                result.action = "ThankYou";
            }
        }

        result.newState = currentState;
        return result;
    }
};

#endif