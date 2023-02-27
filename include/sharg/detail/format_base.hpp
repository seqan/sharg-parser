// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides the format_base struct containing all helper functions
 *        that are needed in all formats.
 */

#pragma once

#include <sharg/auxiliary.hpp>
#include <sharg/config.hpp>
#include <sharg/detail/concept.hpp>
#include <sharg/detail/type_name_as_string.hpp>
#include <sharg/validators.hpp>

#if __has_include(<seqan3/version.hpp>)
#    include <seqan3/version.hpp>
#endif

namespace sharg::detail
{

/*!\brief The format that contains all helper functions needed in all formats.
 * \ingroup parser
 * \remark For a complete overview, take a look at \ref parser
 */
class format_base
{
protected:
    /*!\brief Returns the input type as a string (reflection).
     * \tparam value_type The type whose name is converted std::string.
     * \returns The type of the value as a string.
     */
    template <typename value_type>
    static std::string get_type_name_as_string(value_type const & /**/)
    {
        using type = std::decay_t<value_type>;

        if constexpr (std::is_same_v<type, int8_t>)
            return "signed 8 bit integer";
        else if constexpr (std::is_same_v<type, uint8_t>)
            return "unsigned 8 bit integer";
        else if constexpr (std::is_same_v<type, int16_t>)
            return "signed 16 bit integer";
        else if constexpr (std::is_same_v<type, uint16_t>)
            return "unsigned 16 bit integer";
        else if constexpr (std::is_same_v<type, int32_t>)
            return "signed 32 bit integer";
        else if constexpr (std::is_same_v<type, uint32_t>)
            return "unsigned 32 bit integer";
        else if constexpr (std::is_same_v<type, int64_t>)
            return "signed 64 bit integer";
        else if constexpr (std::is_same_v<type, uint64_t>)
            return "unsigned 64 bit integer";
        else if constexpr (std::is_same_v<type, double>)
            return "double";
        else if constexpr (std::is_same_v<type, float>)
            return "float";
        else if constexpr (std::is_same_v<type, bool>)
            return "bool";
        else if constexpr (std::is_same_v<type, char>)
            return "char";
        else if constexpr (std::is_same_v<type, std::string>)
            return "std::string";
        else if constexpr (std::is_same_v<type, std::filesystem::path>)
            return "std::filesystem::path";
        else
            return sharg::detail::type_name_as_string<value_type>;
    }

    /*!\brief Returns the `value_type` of the input container as a string (reflection).
     * \tparam container_type The container type for which to query it's value_type.
     * \returns The type of the container value_type as a string.
     */
    template <detail::is_container_option container_type>
    static std::string get_type_name_as_string(container_type const & /**/)
    {
        typename container_type::value_type tmp{};
        return get_type_name_as_string(tmp);
    }

    /*!\brief Formats the type of a value for the help page printing.
     * \tparam option_value_type The type of the option value to get the info for.
     * \param[in] value The value to deduct the type from.
     * \returns The type of the value as string.
     */
    template <typename option_value_type>
    static std::string option_type_and_list_info(option_value_type const & value)
    {
        return ("(\\fI" + get_type_name_as_string(value) + "\\fP)");
    }

    /*!\brief Formats the container and its value_type for the help page printing.
     * \tparam container_type A type that must satisfy the sharg::detail::is_container_option.
     * \param[in] container The container to deduct the type from.
     *
     * \returns The type of the container value type as a string, encapsulated in "List of".
     */
    template <detail::is_container_option container_type>
    static std::string option_type_and_list_info(container_type const & container)
    {
        return ("(\\fIList\\fP of \\fI" + get_type_name_as_string(container) + "\\fP)");
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

    /*!\brief Escapes certain characters for correct output.
     * \param[in] original The string containing characters to be escaped.
     * \returns The original string as their corresponding html/xml representation.
     *
     * \details Special characters considered are `"`, `\`, `&`, `<` and `>`.
     */
    std::string escape_special_xml_chars(std::string const & original)
    {
        std::string escaped;
        escaped.reserve(original.size()); // will be at least as long

        for (auto c : original)
        {
            if (c == '"')
                escaped.append("&quot;");
            else if (c == '\'')
                escaped.append("&apos;");
            else if (c == '&')
                escaped.append("&amp;");
            else if (c == '<')
                escaped.append("&lt;");
            else if (c == '>')
                escaped.append("&gt;");
            else
                escaped.push_back(c);
        }

        return escaped;
    }

    /*!\brief Expands multiple one character flag identifiers for pretty help output.
     * \param[in] flag_cluster The string of one character flags.
     * \returns A string that lists all flags as a comma separated list.
     *
     * \details e.g. "-agdg" becomes "-a, -g, -d and -g".
     */
    static std::string expand_multiple_flags(std::string const & flag_cluster)
    {
        std::string tmp;
        auto it{flag_cluster.begin()};

        if (flag_cluster[0] == '-')
            ++it;

        for (; it != flag_cluster.end() - 1; ++it)
            tmp.append({'-', *it, ',', ' '});

        tmp.erase(tmp.find_last_of(',')); // remove last ', '
        tmp.append({'a', 'n', 'd', ' ', '-', flag_cluster[flag_cluster.size() - 1]});

        return tmp;
    }
};

/*!\brief The format that contains all helper functions needed in all formats for
 *        printing the interface description of the application (to std::cout).
 * \ingroup parser
 * \remark For a complete overview, take a look at \ref parser
 */
template <typename derived_type>
class format_help_base : public format_base
{
private:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    format_help_base() = default;                                        //!< Defaulted.
    format_help_base(format_help_base const & pf) = default;             //!< Defaulted.
    format_help_base & operator=(format_help_base const & pf) = default; //!< Defaulted.
    format_help_base(format_help_base &&) = default;                     //!< Defaulted.
    format_help_base & operator=(format_help_base &&) = default;         //!< Defaulted.
    ~format_help_base() = default;                                       //!< Defaulted.

    /*!\brief Initializes a format_help_base object.
     * \param[in] names    A list of subcommands (see \link subcommand_parse subcommand parsing \endlink).
     * \param[in] version_updates Whether the developer disabled version checks when constructing the parser.
     * \param[in] advanced Set to `true` to show advanced options.
     */
    format_help_base(std::vector<std::string> const & names,
                     update_notifications const version_updates,
                     bool const advanced) :
        version_check_dev_decision{version_updates},
        command_names{names},
        show_advanced_options{advanced}
    {}
    //!\}

public:
    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::parser::add_option
     */
    template <typename option_type, typename config_type>
    void add_option(option_type & value, config_type const & config)
    {
        std::string id = prep_id_for_help(config.short_id, config.long_id) + " " + option_type_and_list_info(value);
        std::string info{config.description};
        if (config.default_message.empty())
            info += ((config.required) ? std::string{" "} : detail::to_string(" Default: ", value, ". "));
        else
            info += detail::to_string(" Default: ", config.default_message, ". ");

        info += config.validator.get_help_page_message();

        store_help_page_element(
            [this, id, info]()
            {
                derived_t().print_list_item(id, info);
            },
            config);
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::parser::add_flag
     */
    template <typename config_type>
    void add_flag(bool & SHARG_DOXYGEN_ONLY(value), config_type const & config)
    {
        store_help_page_element(
            [this, id = prep_id_for_help(config.short_id, config.long_id), description = config.description]()
            {
                derived_t().print_list_item(id, description);
            },
            config);
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::parser::add_positional_option
     */
    template <typename option_type, typename config_type>
    void add_positional_option(option_type & value, config_type const & config)
    {
        positional_option_calls.push_back(
            [this, &value, description = config.description, validator = config.validator]()
            {
                ++positional_option_count;
                derived_t().print_list_item(detail::to_string("\\fBARGUMENT-",
                                                              positional_option_count,
                                                              "\\fP ",
                                                              option_type_and_list_info(value)),
                                            description +
                                                // a list at the end may be empty and thus have a default value
                                                ((detail::is_container_option<option_type>)
                                                     ? detail::to_string(" Default: ", value, ". ")
                                                     : std::string{" "})
                                                + validator.get_help_page_message());
            });
        // clang-format on
    }

    /*!\brief Initiates the printing of the help page to std::cout.
     * \param[in] parser_meta The meta information that are needed for a detailed help page.
     */
    void parse(parser_meta_data & parser_meta)
    {
        meta = parser_meta;

        derived_t().print_header();

        if (!meta.synopsis.empty())
        {
            derived_t().print_section("Synopsis");
            derived_t().print_synopsis();
        }

        if (!meta.description.empty())
        {
            derived_t().print_section("Description");
            for (auto desc : meta.description)
                print_line(desc);
        }

        if (!command_names.empty())
        {
            derived_t().print_section("Subcommands");
            derived_t().print_line("This program must be invoked with one of the following subcommands:", false);
            for (std::string const & name : command_names)
                derived_t().print_line("- \\fB" + name + "\\fP", false);
            derived_t().print_line("See the respective help page for further details (e.g. by calling " + meta.app_name
                                       + " " + command_names[0] + " -h).",
                                   true);
            derived_t().print_line("The following options below belong to the top-level parser and need to be "
                                   "specified \\fBbefore\\fP the subcommand key word. Every argument after the "
                                   "subcommand key word is passed on to the corresponding sub-parser.",
                                   true);
        }

        // add positional options if specified
        if (!positional_option_calls.empty())
            derived_t().print_section("Positional Arguments");

        // each call will evaluate the function derived_t().print_list_item()
        for (auto f : positional_option_calls)
            f();

        // There are always options because of the common options
        derived_t().print_section("Options");

        // each call will evaluate the function derived_t().print_list_item()
        for (auto f : parser_set_up_calls)
            f();

        // print Common options after developer options
        derived_t().print_subsection("Common options");
        derived_t().print_list_item("\\fB-h\\fP, \\fB--help\\fP", "Prints the help page.");
        derived_t().print_list_item("\\fB-hh\\fP, \\fB--advanced-help\\fP",
                                    "Prints the help page including advanced options.");
        derived_t().print_list_item("\\fB--version\\fP", "Prints the version information.");
        derived_t().print_list_item("\\fB--copyright\\fP", "Prints the copyright/license information.");
        derived_t().print_list_item("\\fB--export-help\\fP (std::string)",
                                    "Export the help page information. Value must be one of [html, man].");
        if (version_check_dev_decision == update_notifications::on)
            derived_t().print_list_item("\\fB--version-check\\fP (bool)",
                                        "Whether to check for the newest app version. Default: true.");

        if (!meta.examples.empty())
        {
            derived_t().print_section("Examples");
            for (auto example : meta.examples)
                print_line(example);
        }

        print_version();

        print_legal();

        derived_t().print_footer();

        std::exit(EXIT_SUCCESS); // program should not continue from here
    }

    /*!\brief Adds a print_section call to parser_set_up_calls.
     * \copydetails sharg::parser::add_section
     */
    void add_section(std::string const & title, bool const advanced_only)
    {
        store_help_page_element(
            [this, title]()
            {
                derived_t().print_section(title);
            },
            advanced_only,
            false /* never hidden */);
    }

    /*!\brief Adds a print_subsection call to parser_set_up_calls.
     * \copydetails sharg::parser::add_subsection
     */
    void add_subsection(std::string const & title, bool const advanced_only)
    {
        store_help_page_element(
            [this, title]()
            {
                derived_t().print_subsection(title);
            },
            advanced_only,
            false /* never hidden */);
    }

    /*!\brief Adds a print_line call to parser_set_up_calls.
     * \copydetails sharg::parser::add_line
     */
    void add_line(std::string const & text, bool is_paragraph, bool const advanced_only)
    {
        store_help_page_element(
            [this, text, is_paragraph]()
            {
                derived_t().print_line(text, is_paragraph);
            },
            advanced_only,
            false /* never hidden */);
    }

    /*!\brief Adds a sharg::print_list_item call to parser_set_up_calls.
     * \copydetails sharg::parser::add_list_item
     */
    void add_list_item(std::string const & key, std::string const & desc, bool const advanced_only)
    {
        store_help_page_element(
            [this, key, desc]()
            {
                derived_t().print_list_item(key, desc);
            },
            advanced_only,
            false /* never hidden */);
    }

    /*!\brief Stores all meta information about the application
     *
     * \details
     *
     * This needs to be a member of format_parse, because it needs to present
     * (not filled) when the parser_set_up_calls vector is filled, since all
     * printing functions need some meta information.
     * The member variable itself is filled when copied over from the parser
     * when calling format_parse::parse. That way all the information needed are
     * there, when the actual printing starts.
     *
     * This function is not private because it is needed for short but nicely
     * formatted (error) output to the command line.
     */
    parser_meta_data meta;

    //!\brief Befriend the derived type so it can access private functions.
    friend derived_type;

protected:
    //!\brief Set on construction and indicates whether the developer deactivated the version check calls completely.
    update_notifications version_check_dev_decision{};

    //!\brief Returns the derived type.
    derived_type & derived_t()
    {
        return static_cast<derived_type &>(*this);
    }

    //!\brief Prints a synopsis in any format.
    void print_synopsis()
    {
        for (unsigned i = 0; i < meta.synopsis.size(); ++i)
        {
            std::string text = "\\fB";
            text.append(meta.synopsis[i]);
            text.insert(text.find_first_of(" \t"), "\\fP");

            derived_t().print_line(text, false);
        }
    }

    /*!\brief Delegates to sharg::print_line(std::string const & text, true) of each format.
     * \param[in] text The text to print.
     */
    void print_line(std::string const & text)
    {
        derived_t().print_line(text, true);
    }

    //!\brief Prints the version information.
    void print_version()
    {
        std::string const version_str{sharg::sharg_version_cstring};

        // Print version, date and url.
        derived_t().print_section("Version");
        derived_t().print_line(derived_t().in_bold("Last update: ") + meta.date, false);
        derived_t().print_line(derived_t().in_bold(meta.app_name + " version: ") + meta.version, false);
        derived_t().print_line(derived_t().in_bold("Sharg version: ") + version_str, false);

#ifdef SEQAN3_VERSION_CSTRING
        std::string const seqan3_version_str{seqan3::seqan3_version_cstring};
        derived_t().print_line(derived_t().in_bold("SeqAn version: ") + seqan3_version_str, false);
#endif

        if (!empty(meta.url))
        {
            derived_t().print_section("Url");
            derived_t().print_line(meta.url, false);
        }
    }

    //!\brief Prints the legal information.
    void print_legal()
    {
        // Print legal stuff
        if ((!empty(meta.short_copyright)) || (!empty(meta.long_copyright)) || (!empty(meta.citation))
            || (!empty(meta.author)) || (!empty(meta.email)))
        {
            derived_t().print_section("Legal");

            if (!empty(meta.short_copyright))
            {
                derived_t().print_line(derived_t().in_bold(meta.app_name + " Copyright: ") + meta.short_copyright,
                                       false);
            }

            if (!empty(meta.author))
            {
                derived_t().print_line(derived_t().in_bold("Author: ") + meta.author, false);
            }

            if (!empty(meta.email))
            {
                derived_t().print_line(derived_t().in_bold("Contact: ") + meta.email, false);
            }

            derived_t().print_line(derived_t().in_bold("SeqAn Copyright: ")
                                       + "2006-2023 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.",
                                   false);

            if (!empty(meta.citation))
            {
                derived_t().print_line(derived_t().in_bold("In your academic works please cite: ") + meta.citation,
                                       false);
            }

            if (!empty(meta.long_copyright))
            {
                derived_t().print_line("For full copyright and/or warranty information see "
                                           + derived_t().in_bold("--copyright") + ".",
                                       false);
            }
        }
    }

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

private:
    /*!\brief Adds a function object to parser_set_up_calls **if** the annotation in `config` does not prevent it.
     * \param[in] printer The invokable that, if added to `parser_set_up_calls`, prints information to the help page.
     * \param[in] advanced Whether the help page element was configured to be hidden.
     * \param[in] hidden Whether the help page element was configured to be hidden.
     *
     * \details
     *
     * If `hidden = true`, the information is never added to the help page.
     * If `advanced = true`, the information is only added to the help page if
     * the advanced help page has been queried on the command line (`show_advanced_options == true`).
     */
    void store_help_page_element(std::function<void()> printer, bool const advanced, bool const hidden)
    {
        if (!(hidden) && (!(advanced) || show_advanced_options))
            parser_set_up_calls.push_back(std::move(printer));
    }

    /*!\brief Adds a function object to parser_set_up_calls **if** the annotation in `config` does not prevent it.
     * \param[in] printer The invokable that, if added to `parser_set_up_calls`, prints information to the help page.
     * \param[in] config The sharg::config object to access `config.advanced` and  `config.hidden`.
     *
     * \details
     *
     * If `config.hidden = true`, the information is never added to the help page.
     * If `config.advanced = true`, the information is only added to the help page if
     * the advanced help page has been queried on the command line (`show_advanced_options == true`).
     */
    void store_help_page_element(std::function<void()> printer, config<auto> const & config)
    {
        if (!(config.hidden) && (!(config.advanced) || show_advanced_options))
            parser_set_up_calls.push_back(std::move(printer));
    }
};

} // namespace sharg::detail
