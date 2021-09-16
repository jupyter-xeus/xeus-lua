#############################################################################
# Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and      # 
# Wolf Vollprecht                                                           #
# Copyright (c) 2018, QuantStack                                            #
#                                                                           #
# Distributed under the terms of the BSD 3-Clause License.                  #
#                                                                           #
# The full license is in the file LICENSE, distributed with this software.  #
#############################################################################

import tempfile
import unittest
import jupyter_kernel_test


class XeusLusTests(jupyter_kernel_test.KernelTests):

    kernel_name = "xlua"
    language_name = "lua"

    code_hello_world = "print('hello, world')"

    # code_page_something = "?print"

    completion_samples = [
        {'text': 'pri', 'matches': {'print'}},
        {'text': 're', 'matches': {'repeat', 'require', 'return'}}
    ]

    complete_code_samples = ["print('hello, world')", "function() function() end end"]
    # incomplete_code_samples = ["print('hello", "function() function() end"]
    # invalid_code_samples = ['import = 7q']

    #code_inspect_sample = "open"

    # def test_xeus_lua_history_manager(self):
    #     reply, output_msgs = self.execute_helper(code="assert get_ilua().history_manager is not None")
    #     self.assertEqual(reply['content']['status'], 'ok')

    def test_xeus_lua_stdout(self):
        reply, output_msgs = self.execute_helper(code='io.write(3)')
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stdout')
        self.assertEqual(output_msgs[0]['content']['text'], '3')

    def test_xeus_lua_stderr(self):
        reply, output_msgs = self.execute_helper(code='a!=b')
        self.assertEqual(output_msgs[0]['msg_type'], 'error')
        # self.assertEqual(output_msgs[0]['content']['ename'], 'AttributeError')
        # self.assertEqual(output_msgs[0]['content']['evalue'], "'list' object has no attribute 'push_back'")
        # traceback = output_msgs[0]['content']['traceback']
        # self.assertEqual(
        #     "\u001b[0;31m---------------------------------------------------------------------------\u001b[0m\n\u001b[0;31mAttributeError\u001b[0m                            Traceback (most recent call last)",
        #     traceback[0]
        # )
        # self.assertTrue(
        #     "a = []; a.push_back" in traceback[2]
        # )
        # self.assertEqual(
        #     "\033[0;31mAttributeError\033[0m: 'list' object has no attribute 'push_back'\n\033[0;31m---------------------------------------------------------------------------\033[0m",
        #     traceback[3]
        # )

if __name__ == '__main__':
    unittest.main()
