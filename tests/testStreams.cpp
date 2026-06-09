#include <gtest/gtest.h>

#include "../SequenceStream/SequenceReadOnlyStream.h"
#include "../SequenceStream/SequenceWriteOnlyStream.h"
#include "../FileStream/FileWriteOnlyStream.h"
#include "../FileStream/FileReadOnlyStream.h"

TEST(WriteOnlyStreamTest, WriteToSequence) {
    MutableArraySequence<int> seq;
    SequenceWriteOnlyStream<int> stream(&seq);

    EXPECT_EQ(stream.GetPosition(), 0u);

    stream.Open();

    EXPECT_EQ(stream.Write(10), 1u);
    EXPECT_EQ(stream.GetPosition(), 1u);

    EXPECT_EQ(stream.Write(20), 2u);
    EXPECT_EQ(stream.GetPosition(), 2u);

    stream.Close();

    MutableArraySequence<int>* result = stream.GetSequence();
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->GetLength(), 2);
    EXPECT_EQ(result->Get(0), 10);
    EXPECT_EQ(result->Get(1), 20);
}

TEST(WriteOnlyStreamTest, WriteWithoutOpenThrows) {
    MutableArraySequence<int> seq;
    SequenceWriteOnlyStream<int> stream(&seq);

    EXPECT_THROW(stream.Write(42), std::logic_error);
}

TEST(WriteOnlyStreamTest, WriteAfterCloseThrows) {
    MutableArraySequence<int> seq;
    SequenceWriteOnlyStream<int> stream(&seq);

    stream.Open();
    EXPECT_EQ(stream.Write(1), 1u);
    stream.Close();

    EXPECT_THROW(stream.Write(2), std::logic_error);
}

TEST(WriteOnlyStreamTest, PositionIncreasesOnWrite) {
    MutableArraySequence<int> seq;
    SequenceWriteOnlyStream<int> stream(&seq);

    stream.Open();
    EXPECT_EQ(stream.GetPosition(), 0u);

    EXPECT_EQ(stream.Write(5), 1u);
    EXPECT_EQ(stream.GetPosition(), 1u);

    EXPECT_EQ(stream.Write(6), 2u);
    EXPECT_EQ(stream.GetPosition(), 2u);

    stream.Close();

    MutableArraySequence<int>* result = stream.GetSequence();
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->GetLength(), 2);
    EXPECT_EQ(result->Get(0), 5);
    EXPECT_EQ(result->Get(1), 6);
}

std::string SerializeInt(const int& v) {
    return std::to_string(v);
}

TEST(FileWriteOnlyStreamTest, WriteIntsToFile) {
    FileWriteOnlyStream<int> stream("test_output.txt", SerializeInt);

    EXPECT_EQ(stream.GetPosition(), 0u);

    stream.Open();

    EXPECT_EQ(stream.Write(10), 1u);
    EXPECT_EQ(stream.GetPosition(), 1u);

    EXPECT_EQ(stream.Write(20), 2u);
    EXPECT_EQ(stream.GetPosition(), 2u);

    stream.Close();

    std::ifstream file("test_output.txt");
    ASSERT_TRUE(file.is_open());

    std::string line;
    ASSERT_TRUE(std::getline(file, line));
    EXPECT_EQ(line, "10");

    ASSERT_TRUE(std::getline(file, line));
    EXPECT_EQ(line, "20");

    ASSERT_FALSE(std::getline(file, line));
}

int Stoi(const std::string& s) {
    return std::stoi(s);
}

TEST(FileReadOnlyStreamTest, ReadIntsFromFile) {
    {
        std::ofstream out("test_input.txt");
        ASSERT_TRUE(out.is_open());
        out << "10\n";
        out << "20\n";
    }

    FileReadOnlyStream<int> stream("test_input.txt", Stoi);

    stream.Open();

    EXPECT_FALSE(stream.IsEndOfStream());
    EXPECT_EQ(stream.Read(), 10);
    EXPECT_EQ(stream.GetPosition(), 1u);

    EXPECT_FALSE(stream.IsEndOfStream());
    EXPECT_EQ(stream.Read(), 20);
    EXPECT_EQ(stream.GetPosition(), 2u);

    EXPECT_THROW(stream.Read(), std::runtime_error);
    EXPECT_TRUE(stream.IsEndOfStream());

    stream.Close();
}