#include <gtest/gtest.h>
#include <memory>

#include "../LazySequence/Generator.h"


Option<int> Rule1To3(const MutableArraySequence<int>& m) {
    if (m.GetLength() >= 3) {
        return Option<int>();
    }
    return Option<int>(m.GetLength() + 1);
}

std::shared_ptr<Generator<int>> Make1To3Generator() {
    return std::make_shared<RuleGenerator<int, Option<int>(*)(const MutableArraySequence<int>&)>>(&Rule1To3);
}

Option<int> Rule10To12(const MutableArraySequence<int>& m) {
    if (m.GetLength() >= 3) {
        return Option<int>();
    }
    return Option<int>(m.GetLength() + 10);
}

TEST(GeneratorTest, HasNextIsFalse) {
    EmptyGenerator<int> gen;
    MutableArraySequence<int> materialized;

    EXPECT_FALSE(gen.HasNext(materialized));
    EXPECT_TRUE(gen.TryGetNext(materialized).IsNone());
    EXPECT_THROW(gen.GetNext(materialized), std::out_of_range);
}

TEST(GeneratorTest, SimpleRuleWorks) {
    RuleGenerator<int, Option<int>(*)(const MutableArraySequence<int>&)> gen(&Rule10To12);
    MutableArraySequence<int> materialized;

    EXPECT_TRUE(gen.HasNext(materialized));
    int v1 = gen.GetNext(materialized);
    EXPECT_EQ(v1, 10);
    materialized.Append(v1);

    EXPECT_TRUE(gen.HasNext(materialized));
    int v2 = gen.GetNext(materialized);
    EXPECT_EQ(v2, 11);
    materialized.Append(v2);

    EXPECT_TRUE(gen.HasNext(materialized));
    int v3 = gen.GetNext(materialized);
    EXPECT_EQ(v3, 12);
    materialized.Append(v3);

    EXPECT_FALSE(gen.HasNext(materialized));
    EXPECT_TRUE(gen.TryGetNext(materialized).IsNone());
}

TEST(GeneratorTest, AppendsItemAfterSource) {
    auto source = Make1To3Generator();
    AppendGenerator<int> gen(source, 99);
    MutableArraySequence<int> materialized;

    int v;

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 1);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 2);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 3);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 99);
    materialized.Append(v);

    EXPECT_FALSE(gen.HasNext(materialized));
    EXPECT_TRUE(gen.TryGetNext(materialized).IsNone());
}

TEST(GeneratorTest, PrependsItemBeforeSource) {
    auto source = Make1To3Generator();
    PrependGenerator<int> gen(source, 5);

    MutableArraySequence<int> materialized;
    int v;

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 5);
    materialized.Append(v);   // важно

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 1);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 2);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 3);
    materialized.Append(v);

    EXPECT_FALSE(gen.HasNext(materialized));
}

TEST(GeneratorTest, InsertsInMiddle) {
    auto source = Make1To3Generator();
    InsertGenerator<int> gen(source, 77, 1);

    MutableArraySequence<int> materialized;
    int v;

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 1);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 77);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 2);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 3);
    materialized.Append(v);

    EXPECT_FALSE(gen.HasNext(materialized));
}

TEST(GeneratorTest, NegativeIndexThrowsInCtor) {
    auto source = Make1To3Generator();
    EXPECT_THROW(InsertGenerator<int> gen(source, 10, -1), std::out_of_range);
}

TEST(GeneratorTest, PrependInsertAppendChain) {
    auto base = Make1To3Generator();

    auto prepended = std::make_shared<PrependGenerator<int>>(base, 0);
    auto inserted  = std::make_shared<InsertGenerator<int>>(prepended, 77, 2);
    AppendGenerator<int> gen(inserted, 99);

    MutableArraySequence<int> materialized;
    int v;

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 0);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 1);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 77);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 2);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 3);
    materialized.Append(v);

    ASSERT_TRUE(gen.HasNext(materialized));
    v = gen.GetNext(materialized);
    EXPECT_EQ(v, 99);
    materialized.Append(v);

    EXPECT_FALSE(gen.HasNext(materialized));
}