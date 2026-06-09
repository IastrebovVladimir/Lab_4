#include <gtest/gtest.h>

#include "../SequenceStream/SequenceWriteOnlyStream.h"
#include "../SequenceStream/SequenceReadOnlyStream.h"
#include "../StateMachine/TurnstileStateMachine.h"
#include "../StateMachine/MachineRunner.h"


TEST(MachineRunnerTests, GetStateOnEmptyThrows) {
    TurnstileStateMachine machine;

    auto* inputSeq = new MutableArraySequence<TurnstileEvent>();
    SequenceReadOnlyStream<TurnstileEvent> input(inputSeq);

    auto* outputSeq = new MutableArraySequence<TurnstileTransitionResult>();
    SequenceWriteOnlyStream<TurnstileTransitionResult> output(outputSeq);

    MachineRunner<TurnstileState, TurnstileEvent, TurnstileTransitionResult> runner(machine, input, output);

    EXPECT_NO_THROW(runner.Run());
    EXPECT_THROW(runner.GetState(0), std::out_of_range);
}

TEST(MachineRunnerTests, SingleCoinStateAtZero) {
    TurnstileStateMachine machine;

    auto* inputSeq = new MutableArraySequence<TurnstileEvent>();
    inputSeq->Append(TurnstileEvent::Coin);
    SequenceReadOnlyStream<TurnstileEvent> input(inputSeq);

    auto* outputSeq = new MutableArraySequence<TurnstileTransitionResult>();
    SequenceWriteOnlyStream<TurnstileTransitionResult> output(outputSeq);

    MachineRunner<TurnstileState, TurnstileEvent, TurnstileTransitionResult> runner(machine, input, output);

    EXPECT_NO_THROW(runner.Run());

    EXPECT_EQ(runner.GetState(0), TurnstileState::Unlocked);
    EXPECT_THROW(runner.GetState(1), std::out_of_range);
}

TEST(MachineRunnerTests, StatesByGlobalIndex) {
    TurnstileStateMachine machine;

    auto* inputSeq = new MutableArraySequence<TurnstileEvent>();
    inputSeq->Append(TurnstileEvent::Coin);
    inputSeq->Append(TurnstileEvent::Push);
    inputSeq->Append(TurnstileEvent::Push);
    inputSeq->Append(TurnstileEvent::Coin);
    SequenceReadOnlyStream<TurnstileEvent> input(inputSeq);

    auto* outputSeq = new MutableArraySequence<TurnstileTransitionResult>();
    SequenceWriteOnlyStream<TurnstileTransitionResult> output(outputSeq);

    MachineRunner<TurnstileState, TurnstileEvent, TurnstileTransitionResult> runner(machine, input, output);

    EXPECT_NO_THROW(runner.Run());

    EXPECT_EQ(runner.GetState(0), TurnstileState::Unlocked);
    EXPECT_EQ(runner.GetState(1), TurnstileState::Locked);
    EXPECT_EQ(runner.GetState(2), TurnstileState::Locked);
    EXPECT_EQ(runner.GetState(3), TurnstileState::Unlocked);
    EXPECT_THROW(runner.GetState(4), std::out_of_range);
}

TEST(MachineRunnerTests, TwentyStepsIndices0To19) {
    TurnstileStateMachine machine;

    auto* inputSeq = new MutableArraySequence<TurnstileEvent>();
    for (size_t index = 0; index < 20; index++) {
        inputSeq->Append(TurnstileEvent::Coin);
    }
    SequenceReadOnlyStream<TurnstileEvent> input(inputSeq);

    auto* outputSeq = new MutableArraySequence<TurnstileTransitionResult>();
    SequenceWriteOnlyStream<TurnstileTransitionResult> output(outputSeq);

    MachineRunner<TurnstileState, TurnstileEvent, TurnstileTransitionResult> runner(machine, input, output);

    EXPECT_NO_THROW(runner.Run());

    for (size_t index = 0; index < 20; index++) {
        EXPECT_NO_THROW(runner.GetState(index));
    }

    EXPECT_THROW(runner.GetState(20), std::out_of_range);
}

TEST(MachineRunnerTests, TwentyOneWindow1To20) {
    TurnstileStateMachine machine;

    auto* inputSeq = new MutableArraySequence<TurnstileEvent>();
    for (size_t index = 0; index < 21; index++) {
        inputSeq->Append(TurnstileEvent::Coin);
    }
    SequenceReadOnlyStream<TurnstileEvent> input(inputSeq);

    auto* outputSeq = new MutableArraySequence<TurnstileTransitionResult>();
    SequenceWriteOnlyStream<TurnstileTransitionResult> output(outputSeq);

    MachineRunner<TurnstileState, TurnstileEvent, TurnstileTransitionResult> runner(machine, input, output);

    EXPECT_NO_THROW(runner.Run());

    EXPECT_THROW(runner.GetState(0), std::out_of_range);
    EXPECT_NO_THROW(runner.GetState(1));
    EXPECT_NO_THROW(runner.GetState(20));
    EXPECT_THROW(runner.GetState(21), std::out_of_range);
}

TEST(MachineRunnerTests, TwentyFiveKeepLastTwenty) {
    TurnstileStateMachine machine;

    auto* inputSeq = new MutableArraySequence<TurnstileEvent>();
    for (size_t index = 0; index < 25; index++) {
        inputSeq->Append(TurnstileEvent::Coin);
    }
    SequenceReadOnlyStream<TurnstileEvent> input(inputSeq);

    auto* outputSeq = new MutableArraySequence<TurnstileTransitionResult>();
    SequenceWriteOnlyStream<TurnstileTransitionResult> output(outputSeq);

    MachineRunner<TurnstileState, TurnstileEvent, TurnstileTransitionResult> runner(machine, input, output);

    EXPECT_NO_THROW(runner.Run());

    for (size_t index = 0; index < 5; index++) {
        EXPECT_THROW(runner.GetState(index), std::out_of_range);
    }

    for (size_t index = 5; index < 25; index++) {
        EXPECT_NO_THROW(runner.GetState(index));
    }

    EXPECT_THROW(runner.GetState(25), std::out_of_range);
}

TEST(MachineRunnerTests, ThirtyStatesFileLog) {
    TurnstileStateMachine machine;

    auto* inputSeq = new MutableArraySequence<TurnstileEvent>();
    for (size_t index = 0; index < 30; index++) {
        if (index % 2 == 0) {
            inputSeq->Append(TurnstileEvent::Coin);
        } else {
            inputSeq->Append(TurnstileEvent::Push);
        }
    }
    SequenceReadOnlyStream<TurnstileEvent> input(inputSeq);

    auto* outputSeq = new MutableArraySequence<TurnstileTransitionResult>();
    SequenceWriteOnlyStream<TurnstileTransitionResult> output(outputSeq);

    MachineRunner<TurnstileState, TurnstileEvent, TurnstileTransitionResult> runner(machine, input, output);

    EXPECT_NO_THROW(runner.Run());

    for (size_t index = 0; index < 30; index++) {
        TurnstileState expected = (index % 2 == 0) ? TurnstileState::Unlocked : TurnstileState::Locked;

        EXPECT_EQ(runner.GetStateLog(index), expected);
    }
}