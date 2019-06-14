#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ziopp/upath.h>

TEST(upath, absolute_and_relative) {
	ziopp::upath path{ std::string{ "x" } };
	ASSERT_TRUE(path.relative());
	ASSERT_FALSE(path.absolute());

	path = ziopp::upath{ std::string{ ".." } };
	ASSERT_TRUE(path.relative());
	ASSERT_FALSE(path.absolute());

	path = ziopp::upath{ std::string{ "/x" } };
	ASSERT_FALSE(path.relative());
	ASSERT_TRUE(path.absolute());

	ASSERT_EQ(path.full_name(), path.to_absolute().full_name());

	path = ziopp::upath{};
	ASSERT_TRUE(path.empty());
}

#define TEST_STRING_PAIR_FIXTURE(NAME, PARAMA, PARAMB) class NAME ## _fixture : public ::testing::TestWithParam<std::pair<std::string, std::string>> { \
public: \
void SetUp() override { \
	auto param = GetParam(); \
	PARAMA = param.first; \
	PARAMB = param.second; \
} \
protected: \
	std::string PARAMA; \
	std::string PARAMB; \
}

#define TEST_STRING_PAIR(NAME, PARAMA, PARAMB, TEST, ...) TEST_STRING_PAIR_FIXTURE(NAME, PARAMA, PARAMB);\
TEST_P(NAME ## _fixture, NAME) TEST \
INSTANTIATE_TEST_CASE_P(upath, NAME ## _fixture, ::testing::Values(__VA_ARGS__))

TEST_STRING_PAIR(normalize, pathAsText, expectedResult, {
	ziopp::upath path{ pathAsText };
	ASSERT_EQ(expectedResult, path.full_name());

	// Check Equatable
	ziopp::upath expectedPath{ expectedResult };
	ASSERT_TRUE(expectedPath.equals(path));
	ASSERT_TRUE(path == expectedPath);
	ASSERT_FALSE(path != expectedPath);
},
// Test Empty
std::make_pair(std::string{}, std::string{}),

// Tests with regular paths
std::make_pair(std::string{ "/" }, std::string{ "/" }),
std::make_pair(std::string{ "\\" }, std::string{ "/" }),
std::make_pair(std::string{ "a" }, std::string{ "a" }),
std::make_pair(std::string{ "a/b" }, std::string{ "a/b" }),
std::make_pair(std::string{ "a\\b" }, std::string{ "a/b" }),
std::make_pair(std::string{ "a/b/" }, std::string{ "a/b" }),
std::make_pair(std::string{ "a\\b\\" }, std::string{ "a/b" }),
std::make_pair(std::string{ "a///b/c//d" }, std::string{ "a/b/c/d" }),
std::make_pair(std::string{ "///a///b/c//" }, std::string{ "/a/b/c" }),
std::make_pair(std::string{ "a/b/c" }, std::string{ "a/b/c" }),
std::make_pair(std::string{ "/a/b" }, std::string{ "/a/b" }),

// Tests with ".."
std::make_pair(std::string{ ".." }, std::string{ ".." }),
std::make_pair(std::string{ "../../a/.." }, std::string{ "../.." }),
std::make_pair(std::string{ "a/../c" }, std::string{ "c" }),
std::make_pair(std::string{ "a/b/.." }, std::string{ "a" }),
std::make_pair(std::string{ "a/b/c/../.." }, std::string{ "a" }),
std::make_pair(std::string{ "a/b/c/../../.." }, std::string{}),
std::make_pair(std::string{ "./.." }, std::string{ ".." }),
std::make_pair(std::string{ "../." }, std::string{ ".." }),
std::make_pair(std::string{ "../.." }, std::string{ "../.." }),
std::make_pair(std::string{ "../../" }, std::string{ "../.." }),
std::make_pair(std::string{ ".a" }, std::string{ ".a" }),
std::make_pair(std::string{ ".a/b/.." }, std::string{ ".a" }),
std::make_pair(std::string{ "...a/b../" }, std::string{ "...a/b.." }),
std::make_pair(std::string{ "...a/.." }, std::string{}),
std::make_pair(std::string{ "...a/b/.." }, std::string{ "...a" })
);

class TestCombineFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {
public:
	void SetUp() override {
		auto param = GetParam();
		path1 = std::get<0>(param);
		path2 = std::get<1>(param);
		expectedResult = std::get<2>(param);
	}
protected:
	std::string path1;
	std::string path2;
	std::string expectedResult;
};

TEST_P(TestCombineFixture, combine) {
	auto path = ziopp::upath::combine(path1, path2);
	ASSERT_EQ(expectedResult, (std::string)path);

	path = ziopp::upath{ path1 } / ziopp::upath{ path2 };
	ASSERT_EQ(expectedResult, (std::string)path);

	ziopp::upath expectedPath{ expectedResult };
	ASSERT_TRUE(path.equals(expectedPath));
}

INSTANTIATE_TEST_CASE_P(
	upath,
	TestCombineFixture,
	::testing::Values(
		std::make_tuple(std::string{}, std::string{}, std::string{}),
		std::make_tuple(std::string{ "/" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "\\" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "//" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "\\\\" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "/" }, std::string{ "/" }, std::string{ "/" }),
		std::make_tuple(std::string{ "\\" }, std::string{ "\\" }, std::string{ "/" }),
		std::make_tuple(std::string{ "//" }, std::string{ "//" }, std::string{ "/" }),
		std::make_tuple(std::string{}, std::string{ "/" }, std::string{ "/" }),
		std::make_tuple(std::string{ "a" }, std::string{ "b" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a/b" }, std::string{ "c" }, std::string{ "a/b/c" }),
		std::make_tuple(std::string{}, std::string{ "b" }, std::string{ "b" }),
		std::make_tuple(std::string{ "a" }, std::string{}, std::string{ "a" }),
		std::make_tuple(std::string{ "a/b" }, std::string{}, std::string{ "a/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "b/" }, std::string{ "/a/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "/b" }, std::string{ "/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{}, std::string{ "/a" }),
		std::make_tuple(std::string{ "//a" }, std::string{}, std::string{ "/a" }),
		std::make_tuple(std::string{ "a/" }, std::string{}, std::string{ "a" }),
		std::make_tuple(std::string{ "a//" }, std::string{}, std::string{ "a" }),
		std::make_tuple(std::string{ "a/" }, std::string{ "b" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a/" }, std::string{ "b/" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a//" }, std::string{ "b//" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a" }, std::string{ "../b" }, std::string{ "b" }),
		std::make_tuple(std::string{ "a/../" }, std::string{ "b" }, std::string{ "b" }),
		std::make_tuple(std::string{ "/a/.." }, std::string{ "b" }, std::string{ "/b" }),
		std::make_tuple(std::string{ "/a/.." }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "//a//..//" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "\\a" }, std::string{}, std::string{ "/a" }),
		std::make_tuple(std::string{ "\\\\a" }, std::string{}, std::string{ "/a" })
	));

class TestCombine3Fixture : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string, std::string>> {
public:
	void SetUp() override {
		auto param = GetParam();
		path1 = std::get<0>(param);
		path2 = std::get<1>(param);
		path3 = std::get<2>(param);
		expectedResult = std::get<3>(param);
	}
protected:
	std::string path1;
	std::string path2;
	std::string path3;
	std::string expectedResult;
};

TEST_P(TestCombine3Fixture, combine3) {
	auto path = ziopp::upath::combine(path1, path2, path3);

	ASSERT_EQ(expectedResult, (std::string)path);
	ziopp::upath expectedPath{ expectedResult };
	ASSERT_TRUE(path.equals(expectedPath));
}

INSTANTIATE_TEST_CASE_P(
	upath,
	TestCombine3Fixture,
	::testing::Values(
		std::make_tuple(std::string{}, std::string{}, std::string{}, std::string{}),
		std::make_tuple(std::string{ "a" }, std::string{ "b" }, std::string{ "c" }, std::string{ "a/b/c" }),
		std::make_tuple(std::string{ "a/b" }, std::string{ "c" }, std::string{ "d" }, std::string{ "a/b/c/d" }),
		std::make_tuple(std::string{}, std::string{ "b" }, std::string{}, std::string{ "b" }),
		std::make_tuple(std::string{ "a" }, std::string{}, std::string{}, std::string{ "a" }),
		std::make_tuple(std::string{ "a/b" }, std::string{}, std::string{}, std::string{ "a/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "b/" }, std::string{ "c/" }, std::string{ "/a/b/c" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "/b" }, std::string{ "/c" }, std::string{ "/c" })
	));

TEST_STRING_PAIR(directory, path1, expectedDir, {
	ziopp::upath path{path1};
	const ziopp::upath result = path.directory();
	ASSERT_EQ(expectedDir, result.full_name());
},
std::make_pair(std::string{}, std::string{}),
std::make_pair(std::string{ "/a" }, std::string{ "/" }),
std::make_pair(std::string{ "/a/b" }, std::string{ "/a" }),
std::make_pair(std::string{ "/a/b/c.txt" }, std::string{ "/a/b" }),
std::make_pair(std::string{ "a" }, std::string{}),
std::make_pair(std::string{ "../a" }, std::string{ ".." }),
std::make_pair(std::string{ "../../a/b" }, std::string{ "../../a" })
);

class TestInDirectoryFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string, bool, bool>>
{
public:
	void SetUp() override
	{
		auto param = GetParam();
		path1 = std::get<0>(param);
		directory = std::get<1>(param);
		recursive = std::get<2>(param);
		expected = std::get<3>(param);
	}
protected:
	std::string path1;
	std::string directory;
	bool recursive;
	bool expected;
};

TEST_P(TestInDirectoryFixture, in_directory) {
	ziopp::upath path{path1};
	bool result = path.in_directory(directory, recursive);
	ASSERT_EQ(expected, result);
}

INSTANTIATE_TEST_CASE_P(
	upath,
	TestInDirectoryFixture,
	::testing::Values(
		// Test automatic separator insertion
        std::make_tuple(std::string{ "/a/b/c" }, std::string{ "/a/b" }, false, true),
        std::make_tuple(std::string{ "/a/bc" }, std::string{ "/a/b" }, false, false),

        // Test trailing separator
        std::make_tuple(std::string{ "/a/b/" }, std::string{ "/a" }, false, true),
        std::make_tuple(std::string{ "/a/b" }, std::string{ "/a/" }, false, true),
        std::make_tuple(std::string{ "/a/b/" }, std::string{ "/a/" }, false, true),

        // Test recursive option
        std::make_tuple(std::string{ "/a/b/c" }, std::string{ "/a" }, true, true),
        std::make_tuple(std::string{ "/a/b/c" }, std::string{ "/a" }, false, false),

        // Test relative paths
        std::make_tuple(std::string{ "a/b" }, std::string{ "a" }, false, true),

        // Test exact match
        std::make_tuple(std::string{ "/a/b/" }, std::string{ "/a/b/" }, false, true),
        std::make_tuple(std::string{ "/a/b/" }, std::string{ "/a/b/" }, true, true),
        std::make_tuple(std::string{ "/a/b" }, std::string{ "/a/b" }, false, true),
        std::make_tuple(std::string{ "/a/b" }, std::string{ "/a/b" }, true, true)
	));

TEST_STRING_PAIR(name, path1, expectedName, {
	ziopp::upath path{ path1 };
	const std::string result = path.name();
	ASSERT_EQ(expectedName, result);
},
std::make_pair(std::string{}, std::string{}),
std::make_pair(std::string{ "/" }, std::string{}),
std::make_pair(std::string{ "/a" }, std::string{ "a" }),
std::make_pair(std::string{ "/a/b" }, std::string{ "b" }),
std::make_pair(std::string{ "/a/b/c.txt" }, std::string{ "c.txt" }),
std::make_pair(std::string{ "a" }, std::string{ "a" }),
std::make_pair(std::string{ "../a" }, std::string{ "a" }),
std::make_pair(std::string{ "../../a/b" }, std::string{ "b" })
);

TEST_STRING_PAIR(name_without_extension, path1, expectedName, {
	ziopp::upath path{ path1 };
	const std::string result = path.name_without_extension();
	ASSERT_EQ(expectedName, result);
},
std::make_pair(std::string{}, std::string{}),
std::make_pair(std::string{ "/" }, std::string{}),
std::make_pair(std::string{ "/a" }, std::string{ "a" }),
std::make_pair(std::string{ "/a/b" }, std::string{ "b" }),
std::make_pair(std::string{ "/a/b/c.txt" }, std::string{ "c" }),
std::make_pair(std::string{ "a" }, std::string{ "a" }),
std::make_pair(std::string{ "../a" }, std::string{ "a" }),
std::make_pair(std::string{ "../../a/b" }, std::string{ "b" })
);

TEST(upath, split) {
	ASSERT_THAT(ziopp::upath{}.split(), ::testing::IsEmpty());
	ASSERT_THAT(ziopp::upath{ "/" }.split(), ::testing::IsEmpty());
	ASSERT_THAT(ziopp::upath{ "/a" }.split(), ::testing::ElementsAre("a"));
	ASSERT_THAT(ziopp::upath{ "/a/b/c" }.split(), ::testing::ElementsAre("a", "b", "c"));
	ASSERT_THAT(ziopp::upath{ "a" }.split(), ::testing::ElementsAre("a"));
	ASSERT_THAT(ziopp::upath{ "a/b" }.split(), ::testing::ElementsAre("a", "b"));
	ASSERT_THAT(ziopp::upath{ "a/b/c" }.split(), ::testing::ElementsAre("a", "b", "c"));
}