(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["packages_xeus-kernel-extension_lib_index_js"],{

/***/ 28817:
/*!*********************************************************************************************!*\
  !*** ../../node_modules/worker-loader/dist/cjs.js!../../packages/xeus-kernel/lib/worker.js ***!
  \*********************************************************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (/* binding */ Worker_fn)
/* harmony export */ });
function Worker_fn() {
  return new Worker(__webpack_require__.p + "bundle.worker.js");
}


/***/ }),

/***/ 87389:
/*!*********************************************************!*\
  !*** ../../packages/xeus-kernel-extension/lib/index.js ***!
  \*********************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlite/kernel */ 64831);
/* harmony import */ var _jupyterlite_xeus_kernel__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlite/xeus-kernel */ 25286);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.


/**
 * A plugin to register the Xeus kernel.
 */
const kernel = {
    id: '@jupyterlite/xeus-kernel-extension:kernel',
    autoStart: true,
    requires: [_jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_0__.IKernelSpecs],
    activate: (app, kernelspecs) => {
        kernelspecs.register({
            spec: {
                name: 'Xeus-Lua',
                display_name: 'Xeus-Lua',
                language: 'lua',
                argv: [],
                spec: {
                    argv: [],
                    env: {},
                    display_name: 'Xeus-Lua',
                    language: 'lua',
                    interrupt_mode: 'message',
                    metadata: {}
                },
                resources: {
                    'logo-32x32': 'TODO',
                    'logo-64x64': '/kernelspecs/xeus_small.svg'
                }
            },
            create: async (options) => {
                return new _jupyterlite_xeus_kernel__WEBPACK_IMPORTED_MODULE_1__.XeusKernel({
                    ...options
                });
            }
        });
    }
};
const plugins = [kernel];
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (plugins);


/***/ }),

/***/ 25286:
/*!************************************************!*\
  !*** ../../packages/xeus-kernel/lib/kernel.js ***!
  \************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "XeusKernel": () => (/* binding */ XeusKernel)
/* harmony export */ });
/* harmony import */ var _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlite/kernel */ 65246);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/coreutils */ 66065);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var worker_loader_worker__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! worker-loader!./worker */ 28817);


// import createXeusModule from './xeus_dummy';
// import {XeusInterpreter} from './xeus_interpreter';

class XeusKernel extends _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_1__.BaseKernel {
    constructor(options) {
        super(options);
        this._executeDelegate = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.PromiseDelegate();
        this._worker = new worker_loader_worker__WEBPACK_IMPORTED_MODULE_2__.default();
        this._worker.onmessage = e => {
            this._processWorkerMessage(e.data);
        };
    }
    /**
     * Dispose the kernel.
     */
    dispose() {
        if (this.isDisposed) {
            return;
        }
        super.dispose();
    }
    /**
     * A promise that is fulfilled when the kernel is ready.
     */
    // get ready(): Promise<void> {
    //   return this._ready.promise;
    // }
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
                console.log("input-request", msg.content);
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
            implementation: 'xeus',
            implementation_version: '0.1.0',
            language_info: {
                codemirror_mode: {
                    name: 'lua',
                    version: 3
                },
                file_extension: '.lua',
                mimetype: 'text/x-lua',
                name: 'lua',
                nbconvert_exporter: 'lua',
                pygments_lexer: 'lua',
                version: '5.3'
            },
            protocol_version: '5.3',
            status: 'ok',
            banner: 'Xeus: A WebAssembly-powered dummy kernel backed by xeus',
            help_links: [
                {
                    text: 'Lua (WASM) Kernel',
                    url: 'https://xeus.org'
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
        this._executeDelegate = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.PromiseDelegate();
        this._worker.postMessage({ type, data, parent: this.parent });
        return await this._executeDelegate.promise;
    }
}


/***/ })

}]);
//# sourceMappingURL=packages_xeus-kernel-extension_lib_index_js.968dd99f628690ceaa8b.js.map