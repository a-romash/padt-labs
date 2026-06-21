#include <gtest/gtest.h>
#include <cstdio>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include "../Streams.h"
#include "../../ArraySequence.h"

// --- SequenceReadStream ---

TEST(SequenceReadStream, ReadsSequentially) {
    int d[] = {1, 2, 3};
    MutableArraySequence<int> s(d, 3);
    SequenceReadStream<int> rs(s);

    rs.Open();
    EXPECT_FALSE(rs.IsEndOfStream());
    EXPECT_EQ(rs.Read(), 1);
    EXPECT_EQ(rs.Read(), 2);
    EXPECT_EQ(rs.GetPosition(), 2u);
    EXPECT_EQ(rs.Read(), 3);
    EXPECT_TRUE(rs.IsEndOfStream());
    rs.Close();
}

TEST(SequenceReadStream, ReadBeforeOpenThrows) {
    int d[] = {1};
    MutableArraySequence<int> s(d, 1);
    SequenceReadStream<int> rs(s);
    EXPECT_THROW(rs.Read(), StreamException);
}

TEST(SequenceReadStream, ReadPastEndThrows) {
    int d[] = {1};
    MutableArraySequence<int> s(d, 1);
    SequenceReadStream<int> rs(s);
    rs.Open();
    EXPECT_EQ(rs.Read(), 1);
    EXPECT_THROW(rs.Read(), EndOfStream);
}

TEST(SequenceReadStream, Seek) {
    int d[] = {10, 20, 30};
    MutableArraySequence<int> s(d, 3);
    SequenceReadStream<int> rs(s);
    rs.Open();
    EXPECT_EQ(rs.Seek(1), 1u);
    EXPECT_EQ(rs.Read(), 20);
    EXPECT_THROW(rs.Seek(99), EndOfStream);
    rs.Close();
}

TEST(SequenceReadStream, NullSharedSourceThrows) {
    std::shared_ptr<Sequence<int>> null;
    EXPECT_THROW(SequenceReadStream<int> rs(null), std::invalid_argument);
}

// --- LazySequenceReadStream ---

TEST(LazySequenceReadStream, ReadsAndSeeks) {
    int d[] = {5, 6, 7};
    LazySequence<int> ls(d, 3);
    LazySequenceReadStream<int> rs(ls);

    rs.Open();
    EXPECT_EQ(rs.Read(), 5);
    EXPECT_EQ(rs.Seek(2), 2u);
    EXPECT_EQ(rs.Read(), 7);
    EXPECT_TRUE(rs.IsEndOfStream());
    EXPECT_THROW(rs.Read(), EndOfStream);
    rs.Close();
}

// --- StringReadStream ---

TEST(StringReadStream, TokenizesAndDeserializes) {
    StringReadStream<int> rs("10 20 30", [](const std::string& s) { return std::stoi(s); });
    rs.Open();
    EXPECT_EQ(rs.Read(), 10);
    EXPECT_EQ(rs.Read(), 20);
    EXPECT_EQ(rs.Read(), 30);
    EXPECT_THROW(rs.Read(), EndOfStream);
    EXPECT_TRUE(rs.IsEndOfStream());
    rs.Close();
}

TEST(StringReadStream, SeekResets) {
    StringReadStream<int> rs("1 2 3", [](const std::string& s) { return std::stoi(s); });
    rs.Open();
    EXPECT_EQ(rs.Read(), 1);
    EXPECT_EQ(rs.Read(), 2);
    rs.Seek(0);
    EXPECT_EQ(rs.Read(), 1);
    rs.Close();
}

TEST(StringReadStream, EmptyDeserializerThrows) {
    std::function<int(const std::string&)> empty;
    EXPECT_THROW(StringReadStream<int> rs("1", empty), std::invalid_argument);
}

// --- SequenceWriteStream ---

TEST(SequenceWriteStream, WritesIntoSequence) {
    MutableArraySequence<int> dst;
    SequenceWriteStream<int> ws(dst);

    EXPECT_THROW(ws.Write(1), StreamException);  // before open
    ws.Open();
    EXPECT_EQ(ws.Write(1), 1u);
    EXPECT_EQ(ws.Write(2), 2u);
    ws.Close();

    EXPECT_EQ(dst.GetLength(), 2);
    EXPECT_EQ(dst.Get(0), 1);
    EXPECT_EQ(dst.Get(1), 2);
}

// --- File streams ---

TEST(FileStreams, WriteThenReadRoundTrip) {
    const std::string fname = "lazyseq_stream_test_tmp.txt";

    {
        FileWriteStream<int> ws(fname, [](const int& x) { return std::to_string(x); });
        ws.Open();
        ws.Write(7);
        ws.Write(8);
        ws.Write(9);
        ws.Close();
    }
    {
        FileReadStream<int> rs(fname, [](const std::string& s) { return std::stoi(s); });
        rs.Open();
        EXPECT_EQ(rs.Read(), 7);
        EXPECT_EQ(rs.Read(), 8);
        EXPECT_EQ(rs.Read(), 9);
        EXPECT_THROW(rs.Read(), EndOfStream);
        rs.Close();
    }

    std::remove(fname.c_str());
}

TEST(FileStreams, OpenMissingFileThrows) {
    FileReadStream<int> rs("definitely_missing_file_for_test_xyz.txt",
                           [](const std::string& s) { return std::stoi(s); });
    EXPECT_THROW(rs.Open(), StreamException);
}

TEST(FileStreams, EmptySerializerThrows) {
    std::function<std::string(const int&)> empty;
    EXPECT_THROW(FileWriteStream<int> ws("x.txt", empty), std::invalid_argument);
}
