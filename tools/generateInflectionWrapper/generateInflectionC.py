#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
import argparse
import re
from pathlib import Path

HEADER_TEMPLATE = """#include <inflection/{dir_}{class_}.h>

#include <inflection/{dir_}{class_}.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>
"""

FUNC_PATTERN = r"((?://)?INFLECTION_CAPI (?P<modifier>const |enum )?(?P<return_type>\S+) (?P<func_name_h>[^\(]+)\((?P<args_h>[^)]+)?\);)"

TRY_CATCH_PATTERN = """INFLECTION_CAPI {modifier}{return_type} {func_name_h}({args_h}) {{
    try {{
        {return_}{cast}(npc(({const}{class}*)thisObject)->{func_name}({args}));
    }}
    catch (const ::std::exception& e) {{
        inflection::util::TypeConversionUtils::convert(e, error);
    }}{return_catch}
}}
"""

DELETE_PATTERN = """INFLECTION_CAPI {return_type} {func_name_h}({args_h}) {{
    delete ({class}*)thisObject;
}}
"""

CREATE_PATTERN = """INFLECTION_CAPI {return_type} {func_name_h}({args_h}) {{
    try {{
        return ({return_type})new {class}({args});
    }}
    catch (const ::std::exception& e) {{
        inflection::util::TypeConversionUtils::convert(e, error);
    }}{return_catch}
}}
"""


def return_catch(return_type: str) -> str:
    if return_type == 'void':
        return ''
    elif return_type.startswith('int'):
        return '\n    return -1;'
    elif return_type == 'bool':
        return '\n    return false;'
    elif return_type.startswith('double'):
        return '\n    return 0.0;'
    return '\n    return nullptr;'


def func_pattern(func_name: str) -> str:
    if func_name.endswith('destroy'):
        return DELETE_PATTERN
    elif func_name.startswith('create') or func_name.startswith('clone'):
        return CREATE_PATTERN
    else:
        return TRY_CATCH_PATTERN


def cast_to_c_type(modifier: str, c_type: str) -> str:
    c_type_prefix = '(' + modifier + c_type + ')'
    if c_type.endswith('*'):
        return c_type_prefix
    elif c_type.startswith('M'):
        return c_type_prefix + 'new ' + remap_type(c_type)
    else:
        return ''


def remap_type(c_type: str):
    c_type = re.sub("^IDP", "inflection::dialog::proxy::", c_type)
    c_type = re.sub("^ID", "inflection::dialog::", c_type)
    c_type = re.sub("^IDICT", "inflection::dictionary::", c_type)
    c_type = re.sub("^IR", "inflection::resources::", c_type)
    c_type = re.sub("^IANA", "inflection::analysis::", c_type)
    c_type = re.sub("^IFEAT", "inflection::lang::features::", c_type)
    c_type = re.sub("^ILANG", "inflection::lang::", c_type)
    return c_type


def cast_to_cpp_type(c_type: str, arg: str) -> str:
    if c_type.startswith('I'):
        return f'({remap_type(c_type)}) {arg}'
    else:
        return arg


def convert_to_c(fpath: Path):
    # copy h file
    dirname, fname = fpath.parent, fpath.name
    dir_name = dirname.resolve().name
    dir_ = "" if dir_name == 'inflection' else dir_name + "::"
    class_name = fname.rstrip(".h")
    class_ = "::inflection::" + dir_ + class_name

    data = fpath.read_text()
    data = re.sub(r"#pragma.+?\n", "", data)
    data = re.sub(r"#include.+?\n", "", data)
    data = HEADER_TEMPLATE.format(dir_=dir_name + "/" if dir_name != "inflection" else "", class_=class_name) + data

    data = re.sub(r'INFLECTION_CTYPE\([^(]+\);?', '', data)

    funcs = re.findall(FUNC_PATTERN, data)
    # for each function:
    for func in funcs:
        # surround with try catch
        d: dict[str, str] = {}
        func_s, d["modifier"], d["return_type"], d["func_name_h"], d["args_h"] = func
        if func_s.startswith('//'):
            continue
        d["func_name"] = d["func_name_h"].split('_')[-1]
        args = d["args_h"].split(',')
        d["const"] = "const " if args and "thisObject" in args[0] and args[0].startswith("const ") else ""
        d["args"] = parse_args(args)
        d["class"] = class_
        d["return_"] = "" if d["return_type"] == "void" else "return "
        d["cast"] = cast_to_c_type(d['modifier'], d['return_type'])
        d["return_catch"] = return_catch(d['return_type'])
        out_pattern = func_pattern(d['func_name'])
        out = out_pattern.format(**d)
        data = re.sub(re.escape(func_s), out, data)

    cpath = Path(re.sub(r'\.h', '-c.cpp', str(fpath)))
    cpath.write_text(data)


def parse_args(args):
    if args and "thisObject" in args[0]:
        args = args[1:]
    if args and "error" in args[-1]:
        args = args[:-1]
    args = [arg.strip().split(" ") for arg in args]
    for arg in args:
        if len(arg) >= 2:
            arg[-1] = cast_to_cpp_type(arg[-2], arg[-1])
    return ",".join([arg[-1] for arg in args])


def main():
    parser = argparse.ArgumentParser(description='Generate a naive c implementation for a header file.')
    parser.add_argument('fpaths', metavar='F', nargs='+',
                        help='header file')

    args = parser.parse_args()
    for fpath in args.fpaths:
        convert_to_c(Path(fpath))


if __name__ == '__main__':
    main()
