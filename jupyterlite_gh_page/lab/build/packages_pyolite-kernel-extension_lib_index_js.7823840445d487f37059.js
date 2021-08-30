(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["packages_pyolite-kernel-extension_lib_index_js"],{

/***/ 45383:
/*!************************************************************!*\
  !*** ../../packages/pyolite-kernel-extension/lib/index.js ***!
  \************************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/coreutils */ 78548);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlite/kernel */ 64831);
/* harmony import */ var _jupyterlite_pyolite_kernel__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @jupyterlite/pyolite-kernel */ 46366);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.



/**
 * The default CDN fallback for Pyodide
 */
const PYODIDE_CDN_URL = 'https://cdn.jsdelivr.net/pyodide/v0.18.0/full/pyodide.js';
/**
 * A plugin to register the Pyodide kernel.
 */
const kernel = {
    id: '@jupyterlite/pyolite-kernel-extension:kernel',
    autoStart: true,
    requires: [_jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_1__.IKernelSpecs],
    activate: (app, kernelspecs) => {
        const url = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PageConfig.getOption('pyodideUrl') || PYODIDE_CDN_URL;
        const pyodideUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.isLocal(url)
            ? _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(window.location.origin, url)
            : url;
        kernelspecs.register({
            spec: {
                name: 'python',
                display_name: 'Pyolite',
                language: 'python',
                argv: [],
                spec: {
                    argv: [],
                    env: {},
                    display_name: 'Pyolite',
                    language: 'python',
                    interrupt_mode: 'message',
                    metadata: {}
                },
                resources: {
                    'logo-32x32': 'TODO',
                    'logo-64x64': '/kernelspecs/python.png'
                }
            },
            create: async (options) => {
                return new _jupyterlite_pyolite_kernel__WEBPACK_IMPORTED_MODULE_2__.PyoliteKernel({
                    ...options,
                    pyodideUrl
                });
            }
        });
    }
};
const plugins = [kernel];
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (plugins);


/***/ }),

/***/ 46366:
/*!***************************************************!*\
  !*** ../../packages/pyolite-kernel/lib/kernel.js ***!
  \***************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "PyoliteKernel": () => (/* binding */ PyoliteKernel)
/* harmony export */ });
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/coreutils */ 78548);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @jupyterlite/kernel */ 65246);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @lumino/coreutils */ 9727);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var _worker_raw__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(/*! ./worker?raw */ 5150);
/* harmony import */ var _py_pyolite_dist_pyolite_0_1_0a6_py3_none_any_whl__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(/*! ../py/pyolite/dist/pyolite-0.1.0a6-py3-none-any.whl */ 96070);
/* harmony import */ var _py_widgetsnbextension_dist_widgetsnbextension_3_5_0_py3_none_any_whl__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! ../py/widgetsnbextension/dist/widgetsnbextension-3.5.0-py3-none-any.whl */ 76943);
/* harmony import */ var _py_nbformat_dist_nbformat_4_2_0_py3_none_any_whl__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! ../py/nbformat/dist/nbformat-4.2.0-py3-none-any.whl */ 61544);
/* harmony import */ var _py_ipykernel_dist_ipykernel_5_5_5_py3_none_any_whl__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! ../py/ipykernel/dist/ipykernel-5.5.5-py3-none-any.whl */ 41658);




// TODO: see https://github.com/jupyterlite/jupyterlite/issues/151
// TODO: sync this version with the npm version (despite version mangling)

// TODO: sync this version with the pypi version



/**
 * A kernel that executes Python code with Pyodide.
 */
class PyoliteKernel extends _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_2__.BaseKernel {
    /**
     * Instantiate a new PyodideKernel
     *
     * @param options The instantiation options for a new PyodideKernel
     */
    constructor(options) {
        var _a;
        super(options);
        this._executeDelegate = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_1__.PromiseDelegate();
        this._ready = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_1__.PromiseDelegate();
        const { pyodideUrl } = options;
        const widgetsnbextensionWheel = _py_widgetsnbextension_dist_widgetsnbextension_3_5_0_py3_none_any_whl__WEBPACK_IMPORTED_MODULE_3__;
        const widgetsnbextensionWheelUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(window.location.origin, widgetsnbextensionWheel);
        const nbformatWheel = _py_nbformat_dist_nbformat_4_2_0_py3_none_any_whl__WEBPACK_IMPORTED_MODULE_4__;
        const nbformatWheelUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(window.location.origin, nbformatWheel);
        const ipykernelWheel = _py_ipykernel_dist_ipykernel_5_5_5_py3_none_any_whl__WEBPACK_IMPORTED_MODULE_5__;
        const ipykernelWheelUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(window.location.origin, ipykernelWheel);
        const pyoliteWheel = (_a = options.pyoliteWheel) !== null && _a !== void 0 ? _a : _py_pyolite_dist_pyolite_0_1_0a6_py3_none_any_whl__WEBPACK_IMPORTED_MODULE_6__;
        const pyoliteWheelUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(window.location.origin, pyoliteWheel);
        const indexUrl = pyodideUrl.slice(0, pyodideUrl.lastIndexOf('/') + 1);
        const blob = new Blob([
            [
                `importScripts("${pyodideUrl}");`,
                `var indexURL = "${indexUrl}";`,
                `var _widgetsnbextensionWheelUrl = '${widgetsnbextensionWheelUrl}';`,
                `var _nbformatWheelUrl = '${nbformatWheelUrl}';`,
                `var _ipykernelWheelUrl = '${ipykernelWheelUrl}';`,
                `var _pyoliteWheelUrl = '${pyoliteWheelUrl}';`,
                _worker_raw__WEBPACK_IMPORTED_MODULE_7__
            ].join('\n')
        ]);
        this._worker = new Worker(window.URL.createObjectURL(blob));
        this._worker.onmessage = e => {
            this._processWorkerMessage(e.data);
        };
        this._ready.resolve();
    }
    /**
     * Dispose the kernel.
     */
    dispose() {
        if (this.isDisposed) {
            return;
        }
        console.log(`Dispose worker for kernel ${this.id}`);
        this._worker.terminate();
        super.dispose();
    }
    /**
     * A promise that is fulfilled when the kernel is ready.
     */
    get ready() {
        return this._ready.promise;
    }
    /**
     * Process a message coming from the pyodide web worker.
     *
     * @param msg The worker message to process.
     */
    _processWorkerMessage(msg) {
        var _a, _b, _c, _d, _e, _f, _g;
        switch (msg.type) {
            case 'stream': {
                const bundle = (_a = msg.bundle) !== null && _a !== void 0 ? _a : { name: 'stdout', text: '' };
                this.stream(bundle);
                break;
            }
            case 'input_request': {
                const bundle = (_b = msg.content) !== null && _b !== void 0 ? _b : { prompt: '', password: false };
                this.inputRequest(bundle);
                break;
            }
            case 'reply': {
                const bundle = msg.results;
                this._executeDelegate.resolve(bundle);
                break;
            }
            case 'display_data': {
                const bundle = (_c = msg.bundle) !== null && _c !== void 0 ? _c : { data: {}, metadata: {}, transient: {} };
                this.displayData(bundle);
                break;
            }
            case 'update_display_data': {
                const bundle = (_d = msg.bundle) !== null && _d !== void 0 ? _d : { data: {}, metadata: {}, transient: {} };
                this.updateDisplayData(bundle);
                break;
            }
            case 'clear_output': {
                const bundle = (_e = msg.bundle) !== null && _e !== void 0 ? _e : { wait: false };
                this.clearOutput(bundle);
                break;
            }
            case 'execute_result': {
                const bundle = (_f = msg.bundle) !== null && _f !== void 0 ? _f : { execution_count: 0, data: {}, metadata: {} };
                this.publishExecuteResult(bundle);
                break;
            }
            case 'execute_error': {
                const bundle = (_g = msg.bundle) !== null && _g !== void 0 ? _g : { ename: '', evalue: '', traceback: [] };
                this.publishExecuteError(bundle);
                break;
            }
            case 'comm_msg':
            case 'comm_open':
            case 'comm_close': {
                this.handleComm(msg.type, msg.content, msg.metadata, msg.buffers);
                break;
            }
            default:
                this._executeDelegate.resolve({
                    data: {},
                    metadata: {}
                });
                break;
        }
    }
    /**
     * Handle a kernel_info_request message
     */
    async kernelInfoRequest() {
        const content = {
            implementation: 'pyodide',
            implementation_version: '0.1.0',
            language_info: {
                codemirror_mode: {
                    name: 'python',
                    version: 3
                },
                file_extension: '.py',
                mimetype: 'text/x-python',
                name: 'python',
                nbconvert_exporter: 'python',
                pygments_lexer: 'ipython3',
                version: '3.8'
            },
            protocol_version: '5.3',
            status: 'ok',
            banner: 'Pyolite: A WebAssembly-powered Python kernel backed by Pyodide',
            help_links: [
                {
                    text: 'Python (WASM) Kernel',
                    url: 'https://pyodide.org'
                }
            ]
        };
        return content;
    }
    /**
     * Handle an `execute_request` message
     *
     * @param msg The parent message.
     */
    async executeRequest(content) {
        const result = await this._sendRequestMessageToWorker('execute-request', content);
        return {
            execution_count: this.executionCount,
            ...result
        };
    }
    /**
     * Handle an complete_request message
     *
     * @param msg The parent message.
     */
    async completeRequest(content) {
        return await this._sendRequestMessageToWorker('complete-request', content);
    }
    /**
     * Handle an `inspect_request` message.
     *
     * @param content - The content of the request.
     *
     * @returns A promise that resolves with the response message.
     */
    async inspectRequest(content) {
        return await this._sendRequestMessageToWorker('inspect-request', content);
    }
    /**
     * Handle an `is_complete_request` message.
     *
     * @param content - The content of the request.
     *
     * @returns A promise that resolves with the response message.
     */
    async isCompleteRequest(content) {
        return await this._sendRequestMessageToWorker('is-complete-request', content);
    }
    /**
     * Handle a `comm_info_request` message.
     *
     * @param content - The content of the request.
     *
     * @returns A promise that resolves with the response message.
     */
    async commInfoRequest(content) {
        return await this._sendRequestMessageToWorker('comm-info-request', content);
    }
    /**
     * Send an `input_reply` message.
     *
     * @param content - The content of the reply.
     */
    inputReply(content) {
        this._worker.postMessage({
            type: 'input-reply',
            data: content,
            parent: this.parent
        });
    }
    /**
     * Send an `comm_open` message.
     *
     * @param msg - The comm_open message.
     */
    async commOpen(msg) {
        return await this._sendRequestMessageToWorker('comm-open', msg);
    }
    /**
     * Send an `comm_msg` message.
     *
     * @param msg - The comm_msg message.
     */
    async commMsg(msg) {
        return await this._sendRequestMessageToWorker('comm-msg', msg);
    }
    /**
     * Send an `comm_close` message.
     *
     * @param close - The comm_close message.
     */
    async commClose(msg) {
        return await this._sendRequestMessageToWorker('comm-close', msg);
    }
    /**
     * Send a message to the web worker
     *
     * @param type The message type to send to the worker.
     * @param data The message to send to the worker.
     */
    async _sendRequestMessageToWorker(type, data) {
        this._executeDelegate = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_1__.PromiseDelegate();
        this._worker.postMessage({ type, data, parent: this.parent });
        return await this._executeDelegate.promise;
    }
}


/***/ }),

/***/ 5150:
/*!*******************************************************!*\
  !*** ../../packages/pyolite-kernel/lib/worker.js?raw ***!
  \*******************************************************/
/***/ ((module) => {

"use strict";
module.exports = "\"use strict\";\n/**\n * Store the kernel and interpreter instances.\n */\n// eslint-disable-next-line\n// @ts-ignore: breaks typedoc\nlet kernel;\n// eslint-disable-next-line\n// @ts-ignore: breaks typedoc\nlet interpreter;\n// eslint-disable-next-line\n// @ts-ignore: breaks typedoc\nlet pyodide;\n// eslint-disable-next-line\n// @ts-ignore: breaks typedoc\nlet stdout_stream;\n// eslint-disable-next-line\n// @ts-ignore: breaks typedoc\nlet stderr_stream;\n// eslint-disable-next-line\n// @ts-ignore: breaks typedoc\nlet resolveInputReply;\n/**\n * Load Pyodided and initialize the interpreter.\n */\nasync function loadPyodideAndPackages() {\n    // as of 0.17.0 indexURL must be provided\n    pyodide = await loadPyodide({ indexURL });\n    await pyodide.loadPackage(['micropip']);\n    await pyodide.loadPackage(['matplotlib']);\n    await pyodide.runPythonAsync(`\n    import micropip\n    await micropip.install([\n      'traitlets',\n      '${_widgetsnbextensionWheelUrl}',\n      '${_nbformatWheelUrl}',\n      '${_ipykernelWheelUrl}'\n    ])\n    await micropip.install([\n      '${_pyoliteWheelUrl}'\n    ]);\n    await micropip.install('ipython');\n    import pyolite\n  `);\n    // make copies of these so they don't get garbage collected\n    kernel = pyodide.globals.get('pyolite').kernel_instance.copy();\n    stdout_stream = pyodide.globals.get('pyolite').stdout_stream.copy();\n    stderr_stream = pyodide.globals.get('pyolite').stderr_stream.copy();\n    interpreter = kernel.interpreter.copy();\n    interpreter.send_comm = sendComm;\n    const version = pyodide.globals.get('pyolite').__version__;\n    console.log('Pyolite kernel initialized, version', version);\n}\n/**\n * Recursively convert a Map to a JavaScript object\n * @param The Map object to convert\n */\nfunction mapToObject(obj) {\n    const out = obj instanceof Array ? [] : {};\n    obj.forEach((value, key) => {\n        out[key] =\n            value instanceof Map || value instanceof Array ? mapToObject(value) : value;\n    });\n    return out;\n}\n/**\n * Format the response from the Pyodide evaluation.\n *\n * @param res The result object from the Pyodide evaluation\n */\nfunction formatResult(res) {\n    if (!pyodide.isPyProxy(res)) {\n        return res;\n    }\n    // TODO: this is a bit brittle\n    const m = res.toJs();\n    const results = mapToObject(m);\n    return results;\n}\n// eslint-disable-next-line\n// @ts-ignore: breaks typedoc\nconst pyodideReadyPromise = loadPyodideAndPackages();\n/**\n * Send a comm message to the front-end.\n *\n * @param type The type of the comm message.\n * @param content The content.\n * @param metadata The metadata.\n * @param ident The ident.\n * @param buffers The binary buffers.\n */\nasync function sendComm(type, content, metadata, ident, buffers) {\n    postMessage({\n        type: type,\n        content: formatResult(content),\n        metadata: formatResult(metadata),\n        ident: formatResult(ident),\n        buffers: formatResult(buffers)\n    });\n}\nasync function getpass(prompt) {\n    prompt = typeof prompt === 'undefined' ? '' : prompt;\n    await sendInputRequest(prompt, true);\n    const replyPromise = new Promise(resolve => {\n        resolveInputReply = resolve;\n    });\n    const result = await replyPromise;\n    return result['value'];\n}\nasync function input(prompt) {\n    prompt = typeof prompt === 'undefined' ? '' : prompt;\n    await sendInputRequest(prompt, false);\n    const replyPromise = new Promise(resolve => {\n        resolveInputReply = resolve;\n    });\n    const result = await replyPromise;\n    return result['value'];\n}\n/**\n * Send a input request to the front-end.\n *\n * @param prompt the text to show at the prompt\n * @param password Is the request for a password?\n */\nasync function sendInputRequest(prompt, password) {\n    const content = {\n        prompt,\n        password\n    };\n    postMessage({\n        type: 'input_request',\n        parentHeader: formatResult(kernel._parent_header['header']),\n        content\n    });\n}\n/**\n * Execute code with the interpreter.\n *\n * @param content The incoming message with the code to execute.\n */\nasync function execute(content) {\n    const publishExecutionResult = (prompt_count, data, metadata) => {\n        const bundle = {\n            execution_count: prompt_count,\n            data: formatResult(data),\n            metadata: formatResult(metadata)\n        };\n        postMessage({\n            parentHeader: formatResult(kernel._parent_header['header']),\n            bundle,\n            type: 'execute_result'\n        });\n    };\n    const publishExecutionError = (ename, evalue, traceback) => {\n        const bundle = {\n            ename: ename,\n            evalue: evalue,\n            traceback: traceback\n        };\n        postMessage({\n            parentHeader: formatResult(kernel._parent_header['header']),\n            bundle,\n            type: 'execute_error'\n        });\n    };\n    const clearOutputCallback = (wait) => {\n        const bundle = {\n            wait: formatResult(wait)\n        };\n        postMessage({\n            parentHeader: formatResult(kernel._parent_header['header']),\n            bundle,\n            type: 'clear_output'\n        });\n    };\n    const displayDataCallback = (data, metadata, transient) => {\n        const bundle = {\n            data: formatResult(data),\n            metadata: formatResult(metadata),\n            transient: formatResult(transient)\n        };\n        postMessage({\n            parentHeader: formatResult(kernel._parent_header['header']),\n            bundle,\n            type: 'display_data'\n        });\n    };\n    const updateDisplayDataCallback = (data, metadata, transient) => {\n        const bundle = {\n            data: formatResult(data),\n            metadata: formatResult(metadata),\n            transient: formatResult(transient)\n        };\n        postMessage({\n            parentHeader: formatResult(kernel._parent_header['header']),\n            bundle,\n            type: 'update_display_data'\n        });\n    };\n    const publishStreamCallback = (name, text) => {\n        const bundle = {\n            name: formatResult(name),\n            text: formatResult(text)\n        };\n        postMessage({\n            parentHeader: formatResult(kernel._parent_header['header']),\n            bundle,\n            type: 'stream'\n        });\n    };\n    stdout_stream.publish_stream_callback = publishStreamCallback;\n    stderr_stream.publish_stream_callback = publishStreamCallback;\n    interpreter.display_pub.clear_output_callback = clearOutputCallback;\n    interpreter.display_pub.display_data_callback = displayDataCallback;\n    interpreter.display_pub.update_display_data_callback = updateDisplayDataCallback;\n    interpreter.displayhook.publish_execution_result = publishExecutionResult;\n    interpreter.input = input;\n    interpreter.getpass = getpass;\n    const res = await kernel.run(content.code);\n    const results = formatResult(res);\n    if (results['status'] === 'error') {\n        publishExecutionError(results['ename'], results['evalue'], results['traceback']);\n    }\n    return results;\n}\n/**\n * Complete the code submitted by a user.\n *\n * @param content The incoming message with the code to complete.\n */\nfunction complete(content) {\n    const res = kernel.complete(content.code, content.cursor_pos);\n    const results = formatResult(res);\n    return results;\n}\n/**\n * Inspect the code submitted by a user.\n *\n * @param content The incoming message with the code to inspect.\n */\nfunction inspect(content) {\n    const res = kernel.inspect(content.code, content.cursor_pos, content.detail_level);\n    const results = formatResult(res);\n    return results;\n}\n/**\n * Check code for completeness submitted by a user.\n *\n * @param content The incoming message with the code to check.\n */\nfunction isComplete(content) {\n    const res = kernel.is_complete(content.code);\n    const results = formatResult(res);\n    return results;\n}\n/**\n * Respond to the commInfoRequest.\n *\n * @param content The incoming message with the comm target name.\n */\nfunction commInfo(content) {\n    const res = kernel.comm_info(content.target_name);\n    const results = formatResult(res);\n    return {\n        comms: results,\n        status: 'ok'\n    };\n}\n/**\n * Respond to the commOpen.\n *\n * @param content The incoming message with the comm open.\n */\nfunction commOpen(content) {\n    const res = kernel.comm_manager.comm_open(pyodide.toPy(content));\n    const results = formatResult(res);\n    return results;\n}\n/**\n * Respond to the commMsg.\n *\n * @param content The incoming message with the comm msg.\n */\nfunction commMsg(content) {\n    const res = kernel.comm_manager.comm_msg(pyodide.toPy(content));\n    const results = formatResult(res);\n    return results;\n}\n/**\n * Respond to the commClose.\n *\n * @param content The incoming message with the comm close.\n */\nfunction commClose(content) {\n    const res = kernel.comm_manager.comm_close(pyodide.toPy(content));\n    const results = formatResult(res);\n    return results;\n}\n/**\n * Process a message sent to the worker.\n *\n * @param event The message event to process\n */\nself.onmessage = async (event) => {\n    await pyodideReadyPromise;\n    const data = event.data;\n    let results;\n    const messageType = data.type;\n    const messageContent = data.data;\n    kernel._parent_header = pyodide.toPy(data.parent);\n    console.log(\"on message\", messageType, data);\n    switch (messageType) {\n        case 'execute-request':\n            console.log('Perform execution inside worker', data);\n            results = await execute(messageContent);\n            break;\n        case 'input-reply':\n            resolveInputReply(messageContent);\n            return;\n        case 'inspect-request':\n            results = inspect(messageContent);\n            break;\n        case 'is-complete-request':\n            results = isComplete(messageContent);\n            break;\n        case 'complete-request':\n            results = complete(messageContent);\n            break;\n        case 'comm-info-request':\n            results = commInfo(messageContent);\n            break;\n        case 'comm-open':\n            results = commOpen(messageContent);\n            break;\n        case 'comm-msg':\n            results = commMsg(messageContent);\n            break;\n        case 'comm-close':\n            results = commClose(messageContent);\n            break;\n        default:\n            break;\n    }\n    const reply = {\n        parentHeader: data.parent['header'],\n        type: 'reply',\n        results\n    };\n    postMessage(reply);\n};\n";

/***/ }),

/***/ 41658:
/*!****************************************************************************************!*\
  !*** ../../packages/pyolite-kernel/py/ipykernel/dist/ipykernel-5.5.5-py3-none-any.whl ***!
  \****************************************************************************************/
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {

"use strict";
module.exports = __webpack_require__.p + "ipykernel-5.5.5-py3-none-any.whl";

/***/ }),

/***/ 61544:
/*!**************************************************************************************!*\
  !*** ../../packages/pyolite-kernel/py/nbformat/dist/nbformat-4.2.0-py3-none-any.whl ***!
  \**************************************************************************************/
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {

"use strict";
module.exports = __webpack_require__.p + "nbformat-4.2.0-py3-none-any.whl";

/***/ }),

/***/ 96070:
/*!**************************************************************************************!*\
  !*** ../../packages/pyolite-kernel/py/pyolite/dist/pyolite-0.1.0a6-py3-none-any.whl ***!
  \**************************************************************************************/
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {

"use strict";
module.exports = __webpack_require__.p + "pyolite-0.1.0a6-py3-none-any.whl";

/***/ }),

/***/ 76943:
/*!**********************************************************************************************************!*\
  !*** ../../packages/pyolite-kernel/py/widgetsnbextension/dist/widgetsnbextension-3.5.0-py3-none-any.whl ***!
  \**********************************************************************************************************/
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {

"use strict";
module.exports = __webpack_require__.p + "widgetsnbextension-3.5.0-py3-none-any.whl";

/***/ })

}]);
//# sourceMappingURL=packages_pyolite-kernel-extension_lib_index_js.7823840445d487f37059.js.map