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


class KernelTests(jupyter_kernel_test.KernelTests):

    kernel_name = "xlua"
    language_name = "lua"

    code_hello_world = "hello, world"
    code_page_something = "?"
    completion_samples = [
        {'text': 'H', 'matches': {"Hello", "Hey", "Howdy"}}
    ]
    complete_code_samples = ['hello, world']
    incomplete_code_samples = ["incomplete"]
    invalid_code_samples = ['invalid']
    # code_inspect_sample = "print"


    def test_stdout(self):
        
        reply, output_msgs = self.execute_helper(code='hello, world')
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stdout')
        self.assertEqual(output_msgs[0]['content']['text'], 'hello, world')

    def test_stderr(self):
        reply, output_msgs = self.execute_helper(code='error')
        self.assertEqual(output_msgs[0]['msg_type'], 'stream')
        self.assertEqual(output_msgs[0]['content']['name'], 'stderr')

if __name__ == '__main__':
    unittest.main()
