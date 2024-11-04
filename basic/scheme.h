#pragma once

#include "parser.h"

#include <sstream>
#include <vector>
#include <limits>
#include <algorithm>

std::shared_ptr<Object> ReadAll(const std::string& str);

void OutputFirst(std::shared_ptr<Object> tree, std::string& ans);

void OutputSecond(std::shared_ptr<Object> tree, std::string& ans);

class Function;

class QuoteFunction;

class IsNumberFunction;

class EqualFunction;

class MoreFunction;

class LessFunction;

class MoreOrEqualFunction;

class LessOrEqualFunction;

class SumFunction;

class SubstitutionFunction;

class MultiplicationFunction;

class DivideFunction;

class MaxFunction;

class MinFunction;

class AbsFunction;

class IsBooleanFunction;

class NotFunctioon;

class AndFunction;

class OrFunction;

class IsPairFunction;

class IsNullFunctioon;

class IsListFunction;

class ConsFunction;

class CarFunction;

class CdrFunction;

class ListFunction;

class ListRefFunction;

class ListTail;

std::unique_ptr<Function> FunctionCreator(const std::string& func_string);

std::shared_ptr<Object> Count(std::shared_ptr<Object> tree);

std::pair<std::shared_ptr<Object>, bool> RealCount(std::shared_ptr<Object> tree,
                                                   bool isquote = false);

class Interpreter {
public:
    std::string Run(const std::string& string);
};
