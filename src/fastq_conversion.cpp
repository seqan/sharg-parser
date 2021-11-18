#include "fastq_conversion.hpp"

#include <seqan3/io/sequence_file/all.hpp>

void convert_fastq(std::filesystem::path fastq_file, std::filesystem::path out)
{
    seqan3::sequence_file_input fin{fastq_file};
    seqan3::sequence_file_output fout{std::cout, seqan3::format_fasta{}};
    if (!out.empty()) // If an output path is given
        fout = out;

    fout = fin; // conversion
}
