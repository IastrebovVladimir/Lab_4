#include <gtest/gtest.h>
#include <memory>

#include "../LazySequence/LazySequence.h"

static Option<int> NaturaleRule(const MutableArraySequence<int>& prefix) {
    return Option<int>(prefix.GetLength() + 1);
}

static Option<int> OmegaTimes10Rule(const MutableArraySequence<int>& prefix) {
    return Option<int>((prefix.GetLength() + 1) * 10);
}

static Option<int> FibRule(const MutableArraySequence<int>& prefix) {
    if (prefix.GetLength() < 2) {
        return Option<int>();
    }
    int lastValue = prefix.Get(prefix.GetLength() - 1);
    int previousValue = prefix.Get(prefix.GetLength() - 2);
    return Option<int>(lastValue + previousValue);
}

static Option<int> FiniteRule(const MutableArraySequence<int>& prefix) {
    if (prefix.GetLength() >= 5) {
        return Option<int>();
    }
    return Option<int>(prefix.GetLast() + 1);
}

TEST(LazySequenceTransfinite, OmegaPlusFiniteConcatOrderType) {
    MutableArraySequence<int> leftPrefix;
    LazySequence<int> infiniteLeft(&NaturaleRule, leftPrefix, true);

    int finiteTailData[] = {100, 200, 300};
    LazySequence<int> finiteTail(finiteTailData, 3);

    std::unique_ptr<Sequence<int>> concatBase(infiniteLeft.Concat(&finiteTail));
    auto* concatenated = dynamic_cast<LazySequence<int>*>(concatBase.get());
    ASSERT_NE(concatenated, nullptr);

    Ordinal orderType = concatenated->GetOrderType();
    EXPECT_EQ(orderType.omegaCoeff, 1u);
    EXPECT_EQ(orderType.finitePart, 3u);

    Ordinal zero = Ordinal::MakeFinite(0);
    Ordinal five = Ordinal::MakeFinite(5);

    EXPECT_EQ(concatenated->GetOrdinal(zero), concatenated->Get(0));
    EXPECT_EQ(concatenated->GetOrdinal(five), concatenated->Get(5));

    Ordinal omega = Ordinal::MakeOmega();
    Ordinal omegaPlus1 = Ordinal::Add(omega, Ordinal::MakeFinite(1));
    Ordinal omegaPlus2 = Ordinal::Add(omega, Ordinal::MakeFinite(2));

    EXPECT_EQ(concatenated->GetOrdinal(omega), 100);
    EXPECT_EQ(concatenated->GetOrdinal(omegaPlus1), 200);
    EXPECT_EQ(concatenated->GetOrdinal(omegaPlus2), 300);
}

TEST(LazySequenceTransfinite, TwoAppendsToInfiniteSequence) {
    MutableArraySequence<int> initialPrefix;
    LazySequence<int> baseInfinite(&NaturaleRule, initialPrefix, true);

    std::unique_ptr<Sequence<int>> afterFirstAppendBase(baseInfinite.Append(100));
    auto* afterFirstAppend = dynamic_cast<LazySequence<int>*>(afterFirstAppendBase.get());
    ASSERT_NE(afterFirstAppend, nullptr);

    std::unique_ptr<Sequence<int>> afterSecondAppendBase(afterFirstAppend->Append(200));
    auto* afterSecondAppend = dynamic_cast<LazySequence<int>*>(afterSecondAppendBase.get());
    ASSERT_NE(afterSecondAppend, nullptr);

    Ordinal orderType = afterSecondAppend->GetOrderType();
    EXPECT_EQ(orderType.omegaCoeff, 1u);
    EXPECT_EQ(orderType.finitePart, 2u);

    Ordinal omega = Ordinal::MakeOmega();
    Ordinal omegaPlus1 = Ordinal::Add(omega, Ordinal::MakeFinite(1));

    EXPECT_EQ(afterSecondAppend->GetOrdinal(omega),      100);
    EXPECT_EQ(afterSecondAppend->GetOrdinal(omegaPlus1), 200);
}

TEST(LazySequenceTransfinite, TwoOmegaConcatOrdinalAccess) {
    MutableArraySequence<int> leftPrefix;
    LazySequence<int> firstOmega(&NaturaleRule, leftPrefix, true);

    MutableArraySequence<int> rightPrefix;
    LazySequence<int> secondOmega(&OmegaTimes10Rule, rightPrefix, true);

    std::unique_ptr<Sequence<int>> concatBase(firstOmega.Concat(&secondOmega));
    auto* concatenated = dynamic_cast<LazySequence<int>*>(concatBase.get());
    ASSERT_NE(concatenated, nullptr);

    Ordinal orderType = concatenated->GetOrderType();
    EXPECT_EQ(orderType.omegaCoeff, 2u);
    EXPECT_EQ(orderType.finitePart, 0u);


    Ordinal omega = Ordinal::MakeOmega();
    Ordinal twoOmega = Ordinal(1, 5);


    EXPECT_EQ(concatenated->GetOrdinal(omega), secondOmega.Get(0));
    EXPECT_EQ(concatenated->GetOrdinal(twoOmega),  secondOmega.Get(5));
}

TEST(LazySequenceTransfinite, CombinedSequence) {
    int finitePrefixData[] = {7, 8};
    LazySequence<int> finitePrefix(finitePrefixData, 2);

    MutableArraySequence<int> firstInfinitePrefix;
    LazySequence<int> firstInfinite(&NaturaleRule, firstInfinitePrefix, true);

    std::unique_ptr<Sequence<int>> firstInfiniteWithTailBase(firstInfinite.Append(100));
    auto* firstInfiniteWithTail = dynamic_cast<LazySequence<int>*>(firstInfiniteWithTailBase.get());
    ASSERT_NE(firstInfiniteWithTail, nullptr);

    MutableArraySequence<int> secondInfinitePrefix;
    LazySequence<int> secondInfinite(&OmegaTimes10Rule, secondInfinitePrefix, true);

    std::unique_ptr<Sequence<int>> secondInfiniteWithTailBase(secondInfinite.Append(200));
    auto* secondInfiniteWithTail = dynamic_cast<LazySequence<int>*>(secondInfiniteWithTailBase.get());
    ASSERT_NE(secondInfiniteWithTail, nullptr);

    std::unique_ptr<Sequence<int>> firstConcatBase(finitePrefix.Concat(firstInfiniteWithTail));
    auto* firstConcat = dynamic_cast<LazySequence<int>*>(firstConcatBase.get());
    ASSERT_NE(firstConcat, nullptr);

    std::unique_ptr<Sequence<int>> secondConcatBase(firstConcat->Concat(secondInfiniteWithTail));
    auto* secondConcat = dynamic_cast<LazySequence<int>*>(secondConcatBase.get());
    ASSERT_NE(secondConcat, nullptr);

    std::unique_ptr<Sequence<int>> finalSequenceBase(secondConcat->Append(300));
    auto* finalSequence = dynamic_cast<LazySequence<int>*>(finalSequenceBase.get());
    ASSERT_NE(finalSequence, nullptr);

    Ordinal orderType = finalSequence->GetOrderType();
    EXPECT_EQ(orderType.omegaCoeff, 2u);
    EXPECT_EQ(orderType.finitePart, 2u);

    Ordinal zero = Ordinal::MakeFinite(0);
    Ordinal one  = Ordinal::MakeFinite(1);
    EXPECT_EQ(finalSequence->GetOrdinal(zero), 7);
    EXPECT_EQ(finalSequence->GetOrdinal(one),  8);

    Ordinal omega = Ordinal::MakeOmega();
    Ordinal omegaPlus1 = Ordinal::Add(omega, Ordinal::MakeFinite(1));
    Ordinal twoOmega = Ordinal(2, 0);
    Ordinal twoOmegaPlus1 = Ordinal::Add(twoOmega, Ordinal::MakeFinite(1));

    EXPECT_EQ(finalSequence->GetOrdinal(omega), 100);
    EXPECT_EQ(finalSequence->GetOrdinal(omegaPlus1), secondInfinite.Get(0));
    EXPECT_EQ(finalSequence->GetOrdinal(twoOmega), 200);
    EXPECT_EQ(finalSequence->GetOrdinal(twoOmegaPlus1), 300);
}

TEST(LazySequenceOperations, PrependOnInfiniteSequence) {
    int fibonacciStartData[] = {1, 1};
    MutableArraySequence<int> fibonacciStart(fibonacciStartData, 2);

    LazySequence<int> fibonacciSequence(&FibRule, fibonacciStart, true);

    std::unique_ptr<Sequence<int>> prependedBase(fibonacciSequence.Prepend(0));
    auto* prepended = dynamic_cast<LazySequence<int>*>(prependedBase.get());
    ASSERT_NE(prepended, nullptr);

    Ordinal zero = Ordinal::MakeFinite(0);
    EXPECT_EQ(prepended->GetOrdinal(zero), prepended->Get(0));

    EXPECT_THROW(prepended->GetLength(), std::logic_error);
}

TEST(LazySequenceOperations, InsertAtPositionOnInfiniteSequence) {
    int fibonacciStartData[] = {1, 1};
    MutableArraySequence<int> fibonacciStart(fibonacciStartData, 2);

    LazySequence<int> fibonacciSequence(&FibRule, fibonacciStart, true);

    std::unique_ptr<Sequence<int>> insertedBase(fibonacciSequence.InsertAt(999, 7));
    auto* inserted = dynamic_cast<LazySequence<int>*>(insertedBase.get());
    ASSERT_NE(inserted, nullptr);

    Ordinal zero = Ordinal::MakeFinite(0);
    EXPECT_EQ(inserted->GetOrdinal(zero), inserted->Get(0));

    EXPECT_THROW(inserted->GetLength(), std::logic_error);
}

TEST(LazySequenceTransfinite, OrderTypeAndOrdinalAccess) {
    int finitePrefixData[] = {1, 2};
    MutableArraySequence<int> finitePrefix(finitePrefixData, 2);

    LazySequence<int> finiteSequence(&FiniteRule, finitePrefix, false);
    EXPECT_EQ(finiteSequence.GetLength(), 5);

    LazySequence<int> omegaSequence(&NaturaleRule, finiteSequence, true);

    Ordinal finiteOrder = finiteSequence.GetOrderType();
    Ordinal omegaOrder = omegaSequence.GetOrderType();

    EXPECT_EQ(finiteOrder.omegaCoeff, 0u);
    EXPECT_EQ(finiteOrder.finitePart, 5u);

    EXPECT_EQ(omegaOrder.omegaCoeff, 1u);
    EXPECT_EQ(omegaOrder.finitePart, 0u);

    Ordinal zero = Ordinal::MakeFinite(0);
    Ordinal four = Ordinal::MakeFinite(4);
    EXPECT_EQ(omegaSequence.GetOrdinal(zero), omegaSequence.Get(0));
    EXPECT_EQ(omegaSequence.GetOrdinal(four), omegaSequence.Get(4));

    std::unique_ptr<Sequence<int>> appendedBase(omegaSequence.Append(777));
    auto* appended = dynamic_cast<LazySequence<int>*>(appendedBase.get());
    ASSERT_NE(appended, nullptr);

    Ordinal appendedOrder = appended->GetOrderType();
    EXPECT_EQ(appendedOrder.omegaCoeff, 1u);
    EXPECT_EQ(appendedOrder.finitePart, 1u);

    Ordinal omega = Ordinal::MakeOmega();
    EXPECT_EQ(appended->GetOrdinal(omega), 777);
}

TEST(LazySequenceTransfinite, FinitePlusThreeInfiniteWithAllOps) {
    int finiteData[] = {7, 8};
    LazySequence<int> finiteSequence(finiteData, 2);

    MutableArraySequence<int> firstPrefix;
    LazySequence<int> firstInfinite(&NaturaleRule, firstPrefix, true);

    std::unique_ptr<Sequence<int>> firstPrependedBase(firstInfinite.Prepend(1000));
    auto* firstPrepended = dynamic_cast<LazySequence<int>*>(firstPrependedBase.get());
    ASSERT_NE(firstPrepended, nullptr);

    std::unique_ptr<Sequence<int>> firstInsertedBase(firstPrepended->InsertAt(1001, 3));
    auto* firstInserted = dynamic_cast<LazySequence<int>*>(firstInsertedBase.get());
    ASSERT_NE(firstInserted, nullptr);

    std::unique_ptr<Sequence<int>> firstReadyBase(firstInserted->Append(1002));
    auto* firstReady = dynamic_cast<LazySequence<int>*>(firstReadyBase.get());
    ASSERT_NE(firstReady, nullptr);

    MutableArraySequence<int> secondPrefix;
    LazySequence<int> secondInfinite(&OmegaTimes10Rule, secondPrefix, true);

    std::unique_ptr<Sequence<int>> secondPrependedBase(secondInfinite.Prepend(2000));
    auto* secondPrepended = dynamic_cast<LazySequence<int>*>(secondPrependedBase.get());
    ASSERT_NE(secondPrepended, nullptr);

    std::unique_ptr<Sequence<int>> secondInsertedBase(secondPrepended->InsertAt(2001, 2));
    auto* secondInserted = dynamic_cast<LazySequence<int>*>(secondInsertedBase.get());
    ASSERT_NE(secondInserted, nullptr);

    std::unique_ptr<Sequence<int>> secondReadyBase(secondInserted->Append(2002));
    auto* secondReady = dynamic_cast<LazySequence<int>*>(secondReadyBase.get());
    ASSERT_NE(secondReady, nullptr);

    int fibonacciPrefixData[] = {1, 1};
    MutableArraySequence<int> fibonacciPrefix(fibonacciPrefixData, 2);
    LazySequence<int> thirdInfinite(&FibRule, fibonacciPrefix, true);

    std::unique_ptr<Sequence<int>> thirdPrependedBase(thirdInfinite.Prepend(3000));
    auto* thirdPrepended = dynamic_cast<LazySequence<int>*>(thirdPrependedBase.get());
    ASSERT_NE(thirdPrepended, nullptr);

    std::unique_ptr<Sequence<int>> thirdInsertedBase(thirdPrepended->InsertAt(3001, 4));
    auto* thirdInserted = dynamic_cast<LazySequence<int>*>(thirdInsertedBase.get());
    ASSERT_NE(thirdInserted, nullptr);

    std::unique_ptr<Sequence<int>> thirdReadyBase(thirdInserted->Append(3002));
    auto* thirdReady = dynamic_cast<LazySequence<int>*>(thirdReadyBase.get());
    ASSERT_NE(thirdReady, nullptr);

    std::unique_ptr<Sequence<int>> firstConcatBase(finiteSequence.Concat(firstReady));
    auto* firstConcat = dynamic_cast<LazySequence<int>*>(firstConcatBase.get());
    ASSERT_NE(firstConcat, nullptr);

    std::unique_ptr<Sequence<int>> secondConcatBase(firstConcat->Concat(secondReady));
    auto* secondConcat = dynamic_cast<LazySequence<int>*>(secondConcatBase.get());
    ASSERT_NE(secondConcat, nullptr);

    std::unique_ptr<Sequence<int>> thirdConcatBase(secondConcat->Concat(thirdReady));
    auto* finalSequence = dynamic_cast<LazySequence<int>*>(thirdConcatBase.get());
    ASSERT_NE(finalSequence, nullptr);

    Ordinal orderType = finalSequence->GetOrderType();
    EXPECT_GE(orderType.omegaCoeff, 3u);

    Ordinal omega = Ordinal::MakeOmega();
    Ordinal twoOmega = Ordinal(2, 0);
    Ordinal threeOmega = Ordinal(3, 0);

    EXPECT_EQ(finalSequence->GetOrdinal(omega), 1002);

    Ordinal omegaPlus1 = Ordinal(1, 1);
    EXPECT_EQ(finalSequence->GetOrdinal(omegaPlus1), 2000);

    Ordinal omegaPlus2 = Ordinal(1, 2);
    EXPECT_EQ(finalSequence->GetOrdinal(omegaPlus2), 10);

    EXPECT_EQ(finalSequence->GetOrdinal(twoOmega), 2002);

    Ordinal twoOmegaPlus1 = Ordinal(2, 1);
    EXPECT_EQ(finalSequence->GetOrdinal(twoOmegaPlus1), 3000);

    EXPECT_EQ(finalSequence->GetOrdinal(threeOmega), 3002);
}