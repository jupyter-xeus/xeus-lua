#############################################################################
# Copyright (c) 2021, Thorsten Beier                                        #
# Copyright (c) 2021, QuantStack                                            #
#                                                                           #
# Distributed under the terms of the BSD 3-Clause License.                  #
#                                                                           #
# The full license is in the file LICENSE, distributed with this software.  #
#############################################################################

import tempfile
import unittest
import jupyter_kernel_test
import pytest
import json




class XeusLuaTests(jupyter_kernel_test.KernelTests):

    kernel_name = "xlua"
    language_name = "lua"

    code_hello_world = "print('hello, world')"

    # code_page_something = "?print"

    completion_samples = [
        {'text': 'pri', 'matches': {'print'}},
        {'text': 're', 'matches': {'repeat', 'require', 'return'}}
    ]

    complete_code_samples = ["print('hello, world')", "function foo() function bar() end end"]
    incomplete_code_samples = ["function foo("]


    code_snippets = [
        R"""{
            a = 1,
            b = 2,
        }
        """,
        "",
        "-- this is a comment",
        R"""function add(a, b)
            return a + b
        end
        add(1, 1)""",
        R"""1+1""",
        R"""a = [[
            this is a multiline string
        ]]"""
    ]

    code_ends_with_expr = [
       R"""function add(a, b)
            return a + b
        end
        add(1, 1)""",
        R"""1+1""",
        R"""-- this is a comment
        1+1""",
        R"""1+1
        -- this is a comment""",
        R"""-- this is a comment
        1+1
        -- this is another comment""",
    ]
    
    code_does_not_end_with_expr = [
       R"""function add(a, b)
            return a + b
        end
        blub = add(1, 1)""",
        R"""1+1""",
        R"""-- this is a comment
        a = 1+1""",
        R"""b = 1+1
        -- this is a comment""",
        R"""-- this is a comment
        b = 1+1
        -- this is another comment""",
    ]


    def assertNormalizedEqualStr(self, actual, expected):
        # remove newline at end
        if actual.endswith('\n'):
            actual = actual[:-1]
        if expected.endswith('\n'):
            expected = expected[:-1]
        self.assertEqual(actual, expected)

    def test_code_snippets(self):
        self.flush_channels()
        for code in self.code_snippets:
            for add_comment_begin in [True, False]:
                for add_comment_end in [True, False]:
                    code_to_run = code
                    if add_comment_begin:
                        code_to_run = "-- this is a comment\n" + code_to_run
                    if add_comment_end:
                        code_to_run = code_to_run + "\n-- this is another comment"
                    reply, output_msgs = self.execute_helper(code=code_to_run)
                    self.assertEqual(reply["content"]["status"], "ok")

    def test_code_ends_with_expr(self):
        self.flush_channels()
        for code in self.code_ends_with_expr:
            reply, output_msgs = self.execute_helper(code=code)
            self.assertEqual(reply["content"]["status"], "ok")
            self.assertEqual(output_msgs[0]['msg_type'], 'execute_result')
            self.assertNormalizedEqualStr(output_msgs[0]['content']['data']['text/plain'], '2')

    

    def test_code_does_not_end_with_expr(self):
        self.flush_channels()
        for code in self.code_does_not_end_with_expr:
            reply, output_msgs = self.execute_helper(code=code)
            self.assertEqual(reply["content"]["status"], "ok")

    def test_print_orignal(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper('ilua.config.printer = "print"\nprint("hello", "world")')
        self.assertEqual(reply["content"]["status"], "ok")
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stdout')
        self.assertEqual(output_msgs[0]['content']['text'], 'hello\tworld\n')


    def test_lua_io_write(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code="io.write('3')")
        self.assertEqual(reply["content"]["status"], "ok")
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stdout')
        self.assertEqual(output_msgs[0]['content']['text'], '3')
    
    def test_lua_to_stdout(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code="io.stdout:write('hello')")
        self.assertEqual(reply["content"]["status"], "ok")
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stdout')
        self.assertEqual(output_msgs[0]['content']['text'], 'hello')            
    
    def test_lua_to_stdout_with_previous_redirect(self):
        self.flush_channels()
        code = R"""
            -- open a tempfile for writing
            local tmpfile = io.open("_xeus_lua_test_temp_output.txt", "w")

            -- redirect stdout to the tempfile
            io.output(tmpfile)

            -- use io.write to write to the tempfile
            io.write("This should go to the tempfile")

            -- close the tempfile
            tmpfile:close() 

            -- reset stdout to the default
            io.stdout:write("first hello\n")
            io.output(io.stdout)



            io.write("second hello\n")
            io.stdout:flush();
        """
        reply, output_msgs = self.execute_helper(code=code)
        print(json.dumps(output_msgs, indent=4, sort_keys=True, default=str))

        # find first stream message 


        self.assertEqual(reply["content"]["status"], "ok")
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stdout')
        self.assertEqual(output_msgs[0]['content']['text'], 'first hello\n')

        self.assertEqual(output_msgs[1]['msg_type'], 'stream')
        self.assertEqual(output_msgs[1]['content']['name'], 'stdout')
        self.assertEqual(output_msgs[1]['content']['text'], 'second hello\n')

    def test_lua_write_to_stderr(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code="io.stderr:write('error')")
        self.assertEqual(reply["content"]["status"], "ok")
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stderr')
        self.assertEqual(output_msgs[0]['content']['text'], 'error')

    def test_lua_stderr(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code='a!=b)')
        self.assertEqual(output_msgs[0]['msg_type'], 'error')

    
    def test_last_value_printing(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code="1+1")
        self.assertEqual(reply["content"]["status"], "ok")
        self.assertEqual(output_msgs[0]['msg_type'], 'execute_result')
        self.assertNormalizedEqualStr(output_msgs[0]['content']['data']['text/plain'], '2')


    def test_last_value_printing_multiline(self):
        self.flush_channels()
        code = R"""
            function add(a, b)
                return a + b
            end
            add(1, 1)
        """
        reply, output_msgs = self.execute_helper(code=code)
        self.assertEqual(reply["content"]["status"], "ok")
        self.assertEqual(output_msgs[0]['msg_type'], 'execute_result')
        self.assertNormalizedEqualStr(output_msgs[0]['content']['data']['text/plain'], '2')
    

    def test_last_value_mime_bundle(self):
        cases = [
            dict(
                code=R"""ilua.display.plain_text("hello")""",
                expected_mime="text/plain",
                expected_data="hello"
            ),
            dict(
                code=R"""ilua.display.html("<b>hello</b>")""",
                expected_mime="text/html",
                expected_data="<b>hello</b>"
            ),
            dict(
                code=R"""ilua.display.markdown("**hello**")""",
                expected_mime="text/markdown",
                expected_data="**hello**"
            ),
            # latex
            dict(
                code=R"""ilua.display.latex("\\frac{1}{2}")""",
                expected_mime="text/latex",
                expected_data="\\frac{1}{2}"
            ),
            # json
            dict(
                code=R"""ilua.display.json({key="value"})""",
                expected_mime="application/json",
                expected_data={"key":"value"}
            ),
        ]
        for case in cases:
            self.flush_channels()
            reply, output_msgs = self.execute_helper(code=case["code"])
            self.assertEqual(reply["content"]["status"], "ok")
            self.assertEqual(output_msgs[0]['msg_type'], 'execute_result')
            d = output_msgs[0]['content']['data']
            
            self.assertEqual(output_msgs[0]['content']['data'][case["expected_mime"]], case["expected_data"])
        
    def test_output_stream_replacement_detail(self):
        self.flush_channels()
        code = R"""
            -- open a tempfile for writing
            local tmpfile = io.open("_xeus_lua_test_temp_output.txt", "w")
            -- redirect stdout to the tempfile
            io.output(tmpfile)
            -- use io.write to write to the tempfile
            io.write("This should go to the tempfile")
            -- close the tempfile
            tmpfile:close()


            -- reset stdout to the default
            io.stdout:write("redirect1")
            io.stdout:flush()
            io.output(io.stdout)
            io.write("redirect2")
        """

        reply, output_msgs = self.execute_helper(code=code)
        self.assertEqual(reply["content"]["status"], "ok")
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['text'], 'redirect1')
        self.assertEqual(output_msgs[0]['content']['name'], 'stdout')

        self.assertEqual(output_msgs[1]['msg_type'], 'stream')
        self.assertEqual(output_msgs[1]['content']['text'], 'redirect2')
        self.assertEqual(output_msgs[1]['content']['name'], 'stdout')
        # read the content of the tempfile and check if it contains the expected text
        with open("_xeus_lua_test_temp_output.txt", "r") as f:
            content = f.read()
            self.assertEqual(content, "This should go to the tempfile")
        
        # delete the tempfile
        import os
        os.remove("_xeus_lua_test_temp_output.txt")
    
    def test_output_stream_replacement(self):
        self.flush_channels()
        code = R"""
            -- open a tempfile for writing
            local tmpfile = io.open("_xeus_lua_test_temp_output.txt", "w")
            -- redirect stdout to the tempfile
            io.output(tmpfile)
            -- use io.write to write to the tempfile
            io.write("This should go to the tempfile")
            -- close the tempfile
            tmpfile:close()

            -- reset stdout to the default
            io.output(io.stdout)
            io.write("This should go to the default stdout")
        """

        reply, output_msgs = self.execute_helper(code=code)

        self.assertEqual(reply["content"]["status"], "ok")
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['text'], 'This should go to the default stdout')
        self.assertEqual(output_msgs[0]['content']['name'], 'stdout')
        # read the content of the tempfile and check if it contains the expected text
        with open("_xeus_lua_test_temp_output.txt", "r") as f:
            content = f.read()
            self.assertEqual(content, "This should go to the tempfile")
        
        # delete the tempfile
        import os
        os.remove("_xeus_lua_test_temp_output.txt")

if __name__ == '__main__':
    unittest.main()
