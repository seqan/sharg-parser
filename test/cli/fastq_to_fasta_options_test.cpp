#include <seqan3/std/ranges>     // range comparisons
#include <string>                // strings
#include <vector>                // vectors

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/core/detail/debug_stream_alphabet.hpp>
#include <seqan3/core/detail/debug_stream_tuple.hpp>
#include <seqan3/core/detail/debug_stream_type.hpp>
#include <seqan3/io/sequence_file/input.hpp>

// Include the EXPECT_RANGE_EQ macro for better information if range elements differ.
#include <seqan3/test/expect_range_eq.hpp>

#include "cli_test.hpp"

// To prevent issues when running multiple CLI tests in parallel, give each CLI test unique names:
struct fastq_to_fasta : public cli_test {};

TEST_F(fastq_to_fasta, no_options)
{
    cli_test_result result = execute_app("app-template");
    std::string expected
    {
        "Fastq-to-Fasta-Converter\n"
        "========================\n"
        "    Try -h or --help for more information.\n"
    };
    EXPECT_EQ(result.exit_code, 0);
    EXPECT_EQ(result.out, expected);
    EXPECT_EQ(result.err, std::string{});
}

TEST_F(fastq_to_fasta, fail_no_argument)
{
    cli_test_result result = execute_app("app-template", "-v");
    std::string expected
    {
        "Parsing error. Not enough positional arguments provided (Need at least 1). "
        "See -h/--help for more information.\n"
    };
    EXPECT_NE(result.exit_code, 0);
    EXPECT_EQ(result.out, std::string{});
    EXPECT_EQ(result.err, expected);
}

TEST_F(fastq_to_fasta, with_argument)
{
    cli_test_result result = execute_app("app-template", data("in.fastq"));
    EXPECT_EQ(result.exit_code, 0);
    EXPECT_EQ(result.out, "> seq1\nACGTTTGATTCGCG\n> seq2\nTCGGGGGATTCGCG\n");
    EXPECT_EQ(result.err, std::string{});
}

TEST_F(fastq_to_fasta, with_argument_verbose)
{
    cli_test_result result = execute_app("app-template", data("in.fastq"), "-v");
    EXPECT_EQ(result.exit_code, 0);
    EXPECT_EQ(result.out, "> seq1\nACGTTTGATTCGCG\n> seq2\nTCGGGGGATTCGCG\n");
    EXPECT_EQ(result.err, "Conversion was a success. Congrats!\n");
}

TEST_F(fastq_to_fasta, with_out_file)
{
    cli_test_result result = execute_app("app-template", data("in.fastq"), "-o", "out.fasta");
    seqan3::sequence_file_input fin{"out.fasta", seqan3::fields<seqan3::field::seq, seqan3::field::id>{}};

    // create records to compare
    using record_type = typename decltype(fin)::record_type;
    using seqan3::operator""_dna5;
    std::vector<record_type> records{};
    records.emplace_back("ACGTTTGATTCGCG"_dna5, std::string{"seq1"});
    records.emplace_back("TCGGGGGATTCGCG"_dna5, std::string{"seq2"});

    EXPECT_RANGE_EQ(fin, records);
    EXPECT_EQ(result.exit_code, 0);
    EXPECT_EQ(result.out, std::string{});
    EXPECT_EQ(result.err, std::string{});
}
