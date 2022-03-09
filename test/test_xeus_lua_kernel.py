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


class XeusLuaTests(jupyter_kernel_test.KernelTests):

    kernel_name = "xlua"
    language_name = "lua"

    code_hello_world = "print('hello, world')"

    # code_page_something = "?print"

    completion_samples = [
        {'text': 'pri', 'matches': {'print'}},
        {'text': 're', 'matches': {'repeat', 'require', 'return'}}
    ]

    complete_code_samples = ["print('hello, world')", "function() function() end end"]

    def test_xeus_lua_stdout(self):
        reply, output_msgs = self.execute_helper(code='io.write(3)')
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stdout')
        self.assertEqual(output_msgs[0]['content']['text'], '3')

    def test_xeus_lua_stderr(self):
        reply, output_msgs = self.execute_helper(code='a!=b)')
        self.assertEqual(output_msgs[0]['msg_type'], 'error')

if __name__ == '__main__':
    unittest.main()
