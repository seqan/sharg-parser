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
class format_tdl : format_base
{
public:
    enum class FileFormat { CTD };

    //!\brief Vector of functions that stores all calls except add_positional_option.
    std::vector<std::function<void()>> parser_set_up_calls;
    //!\brief Vector of functions that stores add_positional_option calls.
    std::vector<std::function<void()>> positional_option_calls; // singled out to be printed on top
    //!\brief Keeps track of the number of positional options
    unsigned positional_option_count{0};
    //!\brief The names of subcommand programs.
    std::vector<std::string> command_names{};
    //!\brief Whether to show advanced options or not.
    bool show_advanced_options{true};


    tdl::ToolInfo info;
    tdl::Param    param;
    FileFormat    fileFormat;

    /*!\brief Stores all meta information about the application
     *
     * \details
     *
     * This needs to be a member of format_parse, because it needs to present
     * (not filled) when the parser_set_up_calls vector is filled, since all
     * printing functions need some meta information.
     * The member variable itself is filled when copied over from the argument_parser
     * when calling format_parse::parse. That way all the information needed are
     * there, when the actual printing starts.
     *
     * This function is not private because it is needed for short but nicely
     * formatted (error) output to the command line.
     */
    argument_parser_meta_data meta;


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
    format_tdl(std::vector<std::string> const & names, bool const advanced = false) : command_names{names}, show_advanced_options{advanced}
    {};
    //!\}

    //!\brief Prints a synopsis in any format.
    void print_synopsis()
    {
        for (unsigned i = 0; i < meta.synopsis.size(); ++i)
        {
            std::string text = "\\fB";
            text.append(meta.synopsis[i]);
            text.insert(text.find_first_of(" \t"), "\\fP");

            print_line(text, false);
        }
    }


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
    void print_line(std::string const & text, bool const line_is_paragraph = true)
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

    template <typename option_type, typename validator_type>
    void add_option(option_type & value,
                    char const short_id,
                    std::string const & long_id,
                    std::string const & desc,
                    option_spec const spec,
                    validator_type && option_validator)
    {
        std::string id = prep_id_for_help(short_id, long_id) + " " + option_type_and_list_info(value);
        std::string info{desc};
        info += ((spec & option_spec::required) ? std::string{" "} : detail::to_string(" Default: ", value, ". "));
        info += option_validator.get_help_page_message();
        store_help_page_element([this, id, info] () {
            param.setValue(text_only(id), "", text_only(info));
        }, spec);
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::argument_parser::add_flag
     */
    void add_flag(bool & SHARG_DOXYGEN_ONLY(value),
                  char const short_id,
                  std::string const & long_id,
                  std::string const & desc,
                  option_spec const spec)
    {
        std::string id = prep_id_for_help(short_id, long_id);
        store_help_page_element([this, id, desc] () {
            param.setValue(text_only(id), "", text_only(desc));
        }, spec);
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::argument_parser::add_positional_option
     */
    template <typename option_type, typename validator_type>
    void add_positional_option(option_type & value,
                               std::string const & desc,
                               validator_type & option_validator)
    {
        std::string msg = option_validator.get_help_page_message();

        positional_option_calls.push_back([this, &value, desc, msg] ()
        {
            ++positional_option_count;
            auto id   = std::to_string(positional_option_count) + " " + option_type_and_list_info(value);
            auto info = desc +
                            // a list at the end may be empty and thus have a default value
                            ((detail::is_container_option<option_type>)
                                ? detail::to_string(" Default: ", value, ". ")
                                : std::string{" "}) +
                            msg;

            param.setValue(text_only(id), "", text_only(info));
        });
    }

    /*!\brief Formats the option/flag identifier pair for the help page printing.
     * \param[in] short_id The short identifier of the option/flag.
     * \param[in] long_id  The long identifier of the option/flag.
     * \returns The name of the short and long id, prepended with (double)dash.
     *
     * \details  e.g. "-i,--integer", "-i", or "--integer".
     */
    static std::string prep_id_for_help(char const short_id, std::string const & long_id)
    {
        // Build list item term.
        std::string term;
        if (short_id != '\0')
            term = "\\fB-" + std::string(1, short_id) + "\\fP";

        if (short_id != '\0' && !long_id.empty())
            term.append(", ");

        if (!long_id.empty())
            term.append("\\fB--" + long_id + "\\fP");

        return term;
    }


    /*!\brief Initiates the printing of the help page to std::cout.
     * \param[in] parser_meta The meta information that are needed for a detailed help page.
     */
    void parse(argument_parser_meta_data & parser_meta)
    {
        meta = parser_meta;

        print_header();

        if (!meta.synopsis.empty())
        {
            print_section("Synopsis");
            print_synopsis();
        }

        if (!meta.description.empty())
        {
            print_section("Description");
            for (auto desc : meta.description)
                print_line(desc);
        }

        if (!command_names.empty())
        {
            print_section("Subcommands");
            print_line("This program must be invoked with one of the following subcommands:", false);
            for (std::string const & name : command_names)
                print_line("- \\fB" + name + "\\fP", false);
            print_line("See the respective help page for further details (e.g. by calling " +
                                   meta.app_name + " " + command_names[0] + " -h).", true);
            print_line("The following options below belong to the top-level parser and need to be "
                       "specified \\fBbefore\\fP the subcommand key word. Every argument after the "
                       "subcommand key word is passed on to the corresponding sub-parser.", true);
        }

        // add positional options if specified
        if (!positional_option_calls.empty())
            print_section("Positional Arguments");

        // each call will evaluate the function derived_t().print_list_item()
        for (auto f : positional_option_calls)
            f();

        // add options and flags if specified
        if (!parser_set_up_calls.empty())
            print_section("Options");

        // each call will evaluate the function derived_t().print_list_item()
        for (auto f : parser_set_up_calls)
            f();

        if (!meta.examples.empty())
        {
            print_section("Examples");
            for (auto example : meta.examples)
                print_line(example);
        }

        //print_version();

        //print_legal();

        print_footer();

        std::exit(EXIT_SUCCESS); // program should not continue from here
    }

    /*!\brief Adds a print_section call to parser_set_up_calls.
     * \copydetails sharg::argument_parser::add_section
     */
    void add_section(std::string const & title, option_spec const spec)
    {
        store_help_page_element([this, title] () { print_section(title); }, spec);
    }

    /*!\brief Adds a print_subsection call to parser_set_up_calls.
     * \copydetails sharg::argument_parser::add_subsection
     */
    void add_subsection(std::string const & title, option_spec const spec)
    {
        store_help_page_element([this, title] () { print_subsection(title); }, spec);
    }

    /*!\brief Adds a print_line call to parser_set_up_calls.
     * \copydetails sharg::argument_parser::add_line
     */
    void add_line(std::string const & text, bool is_paragraph, option_spec const spec)
    {
        store_help_page_element([this, text, is_paragraph] () { print_line(text, is_paragraph); }, spec);
    }

    /*!\brief Adds a sharg::print_list_item call to parser_set_up_calls.
     * \copydetails sharg::argument_parser::add_list_item
     */
    void add_list_item(std::string const & key, std::string const & desc, option_spec const spec)
    {
        store_help_page_element([this, key, desc] () {
            param.setValue(text_only(key), "", text_only(desc));
        }, spec);
    }
    /*!\brief Adds a function object to parser_set_up_calls **if** the annotation in `spec` does not prevent it.
     * \param[in] printer The invokable that, if added to `parser_set_up_calls`, prints information to the help page.
     * \param[in] spec The option specification deciding whether to add the information to the help page.
     *
     * \details
     *
     * If `spec` equals `sharg::option_spec::hidden`, the information is never added to the help page.
     * If `spec` equals `sharg::option_spec::advanced`, the information is only added to the help page if
     * the advanced help page has been queried on the command line (`show_advanced_options == true`).
     */
    void store_help_page_element(std::function<void()> printer, option_spec const spec)
    {
        if (!(spec & option_spec::hidden) && (!(spec & option_spec::advanced) || show_advanced_options))
            parser_set_up_calls.push_back(std::move(printer));
    }


};

} // namespace sharg
