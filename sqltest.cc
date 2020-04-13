#include "sqltest.hh"
#include <maxbase/string.hh>
#include <boost/spirit/home/x3.hpp>
#include <iostream>
#include <iomanip>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT(sql_parser::SelectVar, name);
BOOST_FUSION_ADAPT_STRUCT(sql_parser::SelectGlob, name);
BOOST_FUSION_ADAPT_STRUCT(sql_parser::SelectFct, name);
BOOST_FUSION_ADAPT_STRUCT(sql_parser::SelectNumber, value);
BOOST_FUSION_ADAPT_STRUCT(sql_parser::SelectString, str);

namespace sql_parser
{

const x3::rule<struct identifier_tag, std::string> identifier = "identifier";
const x3::rule<struct select_var_tag, SelectVar> select_var = "select_var";
const x3::rule<struct select_glob_tag, SelectGlob> select_glob = "select_glob";
const x3::rule<struct select_fct_tag, SelectFct> select_fct = "select_fct";
const x3::rule<struct select_nbr_tag, SelectNumber> select_nbr = "select_nbr";
const x3::rule<struct select_str_tag, SelectString> select_str = "select_str";
const x3::rule<struct select_tag, Select> select = "select";
const x3::rule<struct select_tag, SqlStatement> sql_stmt = "stmt";

const auto identifier_def = x3::lexeme[(x3::alpha | x3::char_('_')) >> *(x3::alnum | x3::char_('_'))];
const auto select_var_def = '@' >> identifier;
const auto select_glob_def = "@@" >> -x3::lexeme[x3::lit("global.") >> identifier];
const auto select_fct_def = identifier >> x3::lit("()");
const auto select_nbr_def = x3::double_;
const auto quote = x3::omit[x3::char_("'")];
// TODO, single or double quote + escape. Need something like qi locals, maybe semantic action + x3::Context
const auto select_str_def = x3::lexeme['"' >> +(x3::char_ - '"') >> '"'];

const auto select_expr = select_var | select_glob | select_fct | select_nbr | select_str;
const auto select_def = x3::no_case["select"] >> select_expr % ",";
const auto sql_stmt_def = select % ';' >> -x3::omit[";"];

BOOST_SPIRIT_DEFINE(identifier, select_var, select_glob, select_fct, select_nbr, select_str, select,
                    sql_stmt);

SqlStatement parse_sql(const std::string& sql)
{
//    auto sql = maxbase::lower_case_copy(sql_cased);     // TODO keep case and use x3::no_case where needed
//    maxbase::trim(sql);
//    std::cout << "sql = " << sql << "\n";

    SqlStatement stmt;
    auto first = begin(sql);
    auto success = phrase_parse(first, end(sql), sql_stmt, x3::space, stmt);

    if (success && first == end(sql))
    {
        return stmt;
    }
    else
    {
        Command error {ParseError {"error"}};
        return SqlStatement {error};
    }
}
}
