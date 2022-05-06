// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides the format_tdl struct and its helper functions.
 */

#pragma once

#include <sharg/detail/format_base.hpp>
#include <tdl/ParamCTDFile.h>

namespace sharg::detail
{

/*!!TODO this is just copied from format_tdl
 * \brief The format that prints the help page information formatted for a man page to std::cout.
 * \ingroup argument_parser
 *
 * \details
 *
 * The help page printing is not done immediately, because the user might not
 * provide meta information, positional options, etc. in the correct order.
 * In addition the needed order would be different from the parse format.
 * Thus the calls are stored (parser_set_up_calls and positional_option_calls)
 * and only evaluated when calling sharg::detail::format_help_base::parse.
 *
 * \remark For a complete overview, take a look at \ref argument_parser
 */
class format_tdl : public format_help_base<format_tdl>
{
    //!\brief The CRTP base class type.
    using base_type = format_help_base<format_tdl>;

    //!\brief Befriend the base class to give access to the private member functions.
    friend base_type;
public:
    enum class FileFormat { CTD };


    tdl::ToolInfo info;
    tdl::Param    param;
    FileFormat    fileFormat;

public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    format_tdl(FileFormat fileFormat) : fileFormat{fileFormat} {}
    format_tdl(format_tdl const & pf) = default;             //!< Defaulted.
    format_tdl & operator=(format_tdl const & pf) = default; //!< Defaulted.
    format_tdl(format_tdl &&) = default;                     //!< Defaulted.
    format_tdl & operator=(format_tdl &&) = default;         //!< Defaulted.
    ~format_tdl() = default;                                 //!< Defaulted.

    //!\copydoc format_help_base(std::vector<std::string> const &, bool const)
    format_tdl(std::vector<std::string> const & names, bool const advanced = false) : base_type{names, advanced}
    {};
    //!\}

private:
    //!\brief Prints a help page header in man page format to std::cout.
    void print_header()
    {
        std::cout << "Some header\n";
    }

    /*!\brief Prints a section title in man page format to std::cout.
     * \param[in] title The title of the section to print.
     */
    void print_section(std::string const & title)
    {
        param.addSection("nokey", title);
        std::cout << "Section: " << title << "\n";
    }

    /*!\brief Prints a subsection title in man page format to std::cout.
     * \param[in] title The title of the subsection to print.
     */
    void print_subsection(std::string const & title)
    {
        std::cout << "subsection: " << title << "\n";
    }

    /*!\brief Prints a help page section in man page format to std::cout.
     *
     * \param[in] text The text to print.
     * \param[in] line_is_paragraph Whether to insert as paragraph
     *            or just a line (only one line break if not a paragraph).
     */
    void print_line(std::string const & text, bool const line_is_paragraph)
    {
        std::cout << "line: " << text << " " << line_is_paragraph << "\n";
    }



    /*!\brief Prints a help page list_item in man page format to std::cout.
     * \param[in] term The key of the key-value pair of the list item.
     * \param[in] desc The value of the key-value pair of the list item.
     *
     * \details
     *
     * A list item is composed of a key (term) and value (desc)
     * and usually used for option identifier-description-pairs.
     */
    void print_list_item(std::string const & term, std::string const & desc)
    {
        param.setValue(text_only(term), "", text_only(desc));
//        ParamEntry entry{term, desc};

        std::cout << "list_item: " << text_only(term) << " " << text_only(desc) << "\n";
    }

    //!\brief Prints a help page footer in man page format.
    void print_footer()
    {
        std::cout << "footer\n\n\n";
        auto info = tdl::ToolInfo {
            .version_     = meta.version,
            .name_        = meta.app_name,
            .docurl_      = meta.url,
            .category_    = "",
            .description_ = meta.short_description,
            .citations_   = {meta.citation},
        };

        if (fileFormat == FileFormat::CTD) {
            tdl::ParamCTDFile file;
            file.writeCTDToStream(&std::cout, param, info);
        } else {
            throw std::runtime_error("unsupported file format (this is a bug)");
        }
    }

    std::string text_only(std::string const & input) const
    {
        std::string result;
        for (auto it = input.begin(); it != input.end(); ++it)
        {
            if (*it != '\\')
            {
                result.push_back(*it);
                continue;
            }


            // Handle escape sequence, we interpret only "\-", "\fI", and "\fB".
            ++it;
            assert(!(it == input.end()));
            if (*it == '-')
            {
                result.push_back(*it);
            }
            else if (*it != 'f')
            {
                result.push_back('\\');
                result.push_back(*it);
            }
            else
            {
                ++it;
                assert(!(it == input.end()));
            }
        }

        return result;
    }


    /*!\brief Format string as in_bold.
     * \param[in] str The input string to format in bold.
     * \returns The string `str` wrapped in bold formatting.
     */
    std::string in_bold(std::string const & str) const
    {
        return str;
    }
};

} // namespace sharg
