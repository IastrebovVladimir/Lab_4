#include <gtest/gtest.h>

#include "../LazySequence/LazySequence.h"

Option<int> NoNextRule(const MutableArraySequence<int>&) {
    return Option<int>();
}

Option<int> FibRule(const MutableArraySequence<int>& m) {
    if (m.GetLength() < 2) {
        return Option<int>();
    }
    int a = m.Get(m.GetLength() - 1);
    int b = m.Get(m.GetLength() - 2);
    return Option<int>(a + b);
}

Option<int> FiniteRule(const MutableArraySequence<int>& m) {
    if (m.GetLength() >= 5) {
        return Option<int>();
    }
    return Option<int>(m.GetLast() + 1);
}

TEST(LazySequenceBasic, EmptySequenceThrows) {
    LazySequence<int> seq;

    EXPECT_THROW(seq.GetFirst(), std::out_of_range);
    EXPECT_THROW(seq.GetLast(), std::out_of_range);
    EXPECT_THROW(seq.Get(0), std::out_of_range);
    EXPECT_EQ(seq.GetMaterializedCount(), 0u);
}

TEST(LazySequenceBasic, ConstructFromArray) {
    int data[] = {1, 2, 3};

    LazySequence<int> seq(data, 3);

    EXPECT_EQ(seq.GetFirst(), 1);
    EXPECT_EQ(seq.GetLast(), 3);
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_EQ(seq.Get(2), 3);
    EXPECT_EQ(seq.GetMaterializedCount(), 3u);
}

TEST(LazySequenceBasic, NegativeIndexThrows) {
    int data[] = {10, 20, 30};
    LazySequence<int> seq(data, 3);

    EXPECT_THROW(seq.Get(-1), std::out_of_range);
}

TEST(LazySequenceBasic, OutOfRangeThrowsForFiniteSequence) {
    int data[] = {10, 20, 30};
    LazySequence<int> seq(data, 3);

    EXPECT_THROW(seq.Get(3), std::out_of_range);
    EXPECT_THROW(seq.Get(100), std::out_of_range);
}

TEST(LazySequenceLazy, PrefixIsCopied) {
    int prefixData[] = {1, 1};
    MutableArraySequence<int> prefix(prefixData, 2);

    LazySequence<int> seq(&NoNextRule, prefix, false);

    EXPECT_EQ(seq.GetMaterializedCount(), 2u);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 1);
    EXPECT_THROW(seq.Get(2), std::out_of_range);
}

TEST(LazySequenceLazy, MaterializesOnDemand) {
    int prefixData[] = {1, 1};
    MutableArraySequence<int> prefix(prefixData, 2);

    LazySequence<int> seq(&FibRule, prefix, true);

    EXPECT_EQ(seq.GetMaterializedCount(), 2u);

    EXPECT_EQ(seq.Get(2), 2);
    EXPECT_EQ(seq.GetMaterializedCount(), 3u);

    EXPECT_EQ(seq.Get(5), 8);
    EXPECT_GE(seq.GetMaterializedCount(), 6u);
}

TEST(LazySequenceLazy, RepeatedAccessDoesNotGrowMaterialization) {
    int prefixData[] = {1, 1};
    MutableArraySequence<int> prefix(prefixData, 2);

    LazySequence<int> seq(&FibRule, prefix, true);

    EXPECT_EQ(seq.Get(5), 8);
    size_t countAfterFirstAccess = seq.GetMaterializedCount();

    EXPECT_EQ(seq.Get(5), 8);
    EXPECT_EQ(seq.GetMaterializedCount(), countAfterFirstAccess);
}

TEST(LazySequenceInfinite, GetLastThrowsForInfiniteSequence) {
    int prefixData[] = {1, 1};
    MutableArraySequence<int> prefix(prefixData, 2);

    LazySequence<int> seq(&FibRule, prefix, true);

    EXPECT_THROW(seq.GetLast(), std::logic_error);
}

TEST(LazySequenceInfinite, GetLengthThrowsForInfiniteSequence) {
    int prefixData[] = {1, 1};
    MutableArraySequence<int> prefix(prefixData, 2);

    LazySequence<int> seq(&FibRule, prefix, true);

    EXPECT_THROW(seq.GetLength(), std::logic_error);
}

TEST(LazySequenceFiniteRule, GetLengthMaterializesEntireSequence) {
    int prefixData[] = {1, 2};
    MutableArraySequence<int> prefix(prefixData, 2);

    LazySequence<int> seq(&FiniteRule, prefix, false);

    EXPECT_EQ(seq.GetLength(), 5);
    EXPECT_EQ(seq.GetMaterializedCount(), 5u);
    EXPECT_EQ(seq.GetLast(), 5);
}

TEST(LazySequenceEnumerator, CurrentBeforeMoveNextThrows) {
    int data[] = {4, 5, 6};
    LazySequence<int> seq(data, 3);

    auto it = seq.GetEnumerator();

    EXPECT_THROW(it.Current(), std::out_of_range);
}

TEST(LazySequenceEnumerator, EnumeratesFiniteSequence) {
    int data[] = {4, 5, 6};
    LazySequence<int> seq(data, 3);

    auto it = seq.GetEnumerator();

    ASSERT_TRUE(it.MoveNext());
    EXPECT_EQ(it.Current(), 4);

    ASSERT_TRUE(it.MoveNext());
    EXPECT_EQ(it.Current(), 5);

    ASSERT_TRUE(it.MoveNext());
    EXPECT_EQ(it.Current(), 6);

    EXPECT_FALSE(it.MoveNext());
}

TEST(LazySequenceEnumerator, CurrentAfterEndThrows) {
    int data[] = {42};
    LazySequence<int> seq(data, 1);

    auto it = seq.GetEnumerator();

    ASSERT_TRUE(it.MoveNext());
    EXPECT_EQ(it.Current(), 42);

    EXPECT_FALSE(it.MoveNext());
    EXPECT_THROW(it.Current(), std::out_of_range);
}

TEST(LazySequenceOperations, AppendWorks) {
    int data[] = {1, 2, 3};
    LazySequence<int> seq(data, 3);

    Sequence<int>* resultBase = seq.Append(4);
    auto* result = dynamic_cast<LazySequence<int>*>(resultBase);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->Get(0), 1);
    EXPECT_EQ(result->Get(1), 2);
    EXPECT_EQ(result->Get(2), 3);
    EXPECT_EQ(result->Get(3), 4);

    delete result;
}

TEST(LazySequenceOperations, PrependWorks) {
    int data[] = {10, 20, 30};
    LazySequence<int> seq(data, 3);

    Sequence<int>* resultBase = seq.Prepend(5);
    auto* result = dynamic_cast<LazySequence<int>*>(resultBase);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->Get(0), 5);
    EXPECT_EQ(result->Get(1), 10);
    EXPECT_EQ(result->Get(2), 20);
    EXPECT_EQ(result->Get(3), 30);

    delete result;
}

TEST(LazySequenceOperations, InsertAtBeginningWorks) {
    int data[] = {10, 20, 30};
    LazySequence<int> seq(data, 3);

    Sequence<int>* resultBase = seq.InsertAt(5, 0);
    auto* result = dynamic_cast<LazySequence<int>*>(resultBase);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->Get(0), 5);
    EXPECT_EQ(result->Get(1), 10);
    EXPECT_EQ(result->Get(2), 20);
    EXPECT_EQ(result->Get(3), 30);

    delete result;
}

TEST(LazySequenceOperations, InsertAtMiddleWorks) {
    int data[] = {10, 30};
    LazySequence<int> seq(data, 2);

    Sequence<int>* resultBase = seq.InsertAt(20, 1);
    auto* result = dynamic_cast<LazySequence<int>*>(resultBase);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->Get(0), 10);
    EXPECT_EQ(result->Get(1), 20);
    EXPECT_EQ(result->Get(2), 30);

    delete result;
}

TEST(LazySequenceOperations, InsertAtEndWorks) {
    int data[] = {10, 20};
    LazySequence<int> seq(data, 2);

    Sequence<int>* resultBase = seq.InsertAt(30, 2);
    auto* result = dynamic_cast<LazySequence<int>*>(resultBase);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->Get(0), 10);
    EXPECT_EQ(result->Get(1), 20);
    EXPECT_EQ(result->Get(2), 30);

    delete result;
}

TEST(LazySequenceOperations, InsertAtNegativeThrows) {
    int data[] = {1, 2, 3};
    LazySequence<int> seq(data, 3);

    EXPECT_THROW(seq.InsertAt(99, -1), std::out_of_range);
}