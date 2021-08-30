(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["packages_p5-kernel-extension_lib_index_js"],{

/***/ 30810:
/*!******************************************************!*\
  !*** ../../packages/javascript-kernel/lib/kernel.js ***!
  \******************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "JavaScriptKernel": () => (/* binding */ JavaScriptKernel)
/* harmony export */ });
/* harmony import */ var _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlite/kernel */ 65246);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/coreutils */ 66065);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__);


/**
 * A kernel that executes code in an IFrame.
 */
class JavaScriptKernel extends _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_1__.BaseKernel {
    /**
     * Instantiate a new JavaScriptKernel
     *
     * @param options The instantiation options for a new JavaScriptKernel
     */
    constructor(options) {
        super(options);
        this._evalFunc = new Function('window', 'code', 'return window.eval(code);');
        this._ready = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.PromiseDelegate();
        // create the main IFrame
        this._iframe = document.createElement('iframe');
        this._iframe.style.visibility = 'hidden';
        this._iframe.style.position = 'absolute';
        // position outside of the page
        this._iframe.style.top = '-100000px';
        this._iframe.onload = async () => {
            await this._initIFrame();
            this._ready.resolve();
            window.addEventListener('message', (e) => {
                const msg = e.data;
                if (msg.event === 'stream') {
                    const content = msg;
                    this.stream(content);
                }
            });
        };
        document.body.appendChild(this._iframe);
    }
    /**
     * Dispose the kernel.
     */
    dispose() {
        if (this.isDisposed) {
            return;
        }
        this._iframe.remove();
        super.dispose();
    }
    /**
     * A promise that is fulfilled when the kernel is ready.
     */
    get ready() {
        return this._ready.promise;
    }
    /**
     * Handle a kernel_info_request message
     */
    async kernelInfoRequest() {
        const content = {
            implementation: 'JavaScript',
            implementation_version: '0.1.0',
            language_info: {
                codemirror_mode: {
                    name: 'javascript'
                },
                file_extension: '.js',
                mimetype: 'text/javascript',
                name: 'javascript',
                nbconvert_exporter: 'javascript',
                pygments_lexer: 'javascript',
                version: 'es2017'
            },
            protocol_version: '5.3',
            status: 'ok',
            banner: 'A JavaScript kernel running in the browser',
            help_links: [
                {
                    text: 'JavaScript Kernel',
                    url: 'https://github.com/jupyterlite/jupyterlite'
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
        const { code } = content;
        try {
            const result = this._eval(code);
            this.publishExecuteResult({
                execution_count: this.executionCount,
                data: {
                    'text/plain': result
                },
                metadata: {}
            });
            return {
                status: 'ok',
                execution_count: this.executionCount,
                user_expressions: {}
            };
        }
        catch (e) {
            const { name, stack, message } = e;
            this.publishExecuteError({
                ename: name,
                evalue: message,
                traceback: [stack]
            });
            return {
                status: 'error',
                execution_count: this.executionCount,
                ename: name,
                evalue: message,
                traceback: [stack]
            };
        }
    }
    /**
     * Handle an complete_request message
     *
     * @param msg The parent message.
     */
    async completeRequest(content) {
        var _a, _b;
        // naive completion on window names only
        // TODO: improve and move logic to the iframe
        const vars = this._evalFunc(this._iframe.contentWindow, 'Object.keys(window)');
        const { code, cursor_pos } = content;
        const words = (_a = code.slice(0, cursor_pos).match(/(\w+)$/)) !== null && _a !== void 0 ? _a : [];
        const word = (_b = words[0]) !== null && _b !== void 0 ? _b : '';
        const matches = vars.filter(v => v.startsWith(word));
        return {
            matches,
            cursor_start: cursor_pos - word.length,
            cursor_end: cursor_pos,
            metadata: {},
            status: 'ok'
        };
    }
    /**
     * Handle an `inspect_request` message.
     *
     * @param content - The content of the request.
     *
     * @returns A promise that resolves with the response message.
     */
    async inspectRequest(content) {
        throw new Error('Not implemented');
    }
    /**
     * Handle an `is_complete_request` message.
     *
     * @param content - The content of the request.
     *
     * @returns A promise that resolves with the response message.
     */
    async isCompleteRequest(content) {
        throw new Error('Not implemented');
    }
    /**
     * Handle a `comm_info_request` message.
     *
     * @param content - The content of the request.
     *
     * @returns A promise that resolves with the response message.
     */
    async commInfoRequest(content) {
        throw new Error('Not implemented');
    }
    /**
     * Send an `input_reply` message.
     *
     * @param content - The content of the reply.
     */
    inputReply(content) {
        throw new Error('Not implemented');
    }
    /**
     * Send an `comm_open` message.
     *
     * @param msg - The comm_open message.
     */
    async commOpen(msg) {
        throw new Error('Not implemented');
    }
    /**
     * Send an `comm_msg` message.
     *
     * @param msg - The comm_msg message.
     */
    async commMsg(msg) {
        throw new Error('Not implemented');
    }
    /**
     * Send an `comm_close` message.
     *
     * @param close - The comm_close message.
     */
    async commClose(msg) {
        throw new Error('Not implemented');
    }
    /**
     * Execute code in the kernel IFrame.
     *
     * @param code The code to execute.
     */
    _eval(code) {
        return this._evalFunc(this._iframe.contentWindow, code);
    }
    /**
     * Create a new IFrame
     *
     * @param iframe The IFrame to initialize.
     */
    async _initIFrame() {
        if (!this._iframe.contentWindow) {
            return;
        }
        this._evalFunc(this._iframe.contentWindow, `
        console._log = console.log;
        console._error = console.error;

        window._bubbleUp = function(msg) {
          window.parent.postMessage(msg);
        }

        console.log = function() {
          const args = Array.prototype.slice.call(arguments);
          window._bubbleUp({
            "event": "stream",
            "name": "stdout",
            "text": args.join(' ') + '\\n'
          });
        };
        console.info = console.log;

        console.error = function() {
          const args = Array.prototype.slice.call(arguments);
          window._bubbleUp({
            "event": "stream",
            "name": "stderr",
            "text": args.join(' ') + '\\n'
          });
        };
        console.warn = console.error;

        window.onerror = function(message, source, lineno, colno, error) {
          console.error(message);
        }
      `);
    }
}


/***/ }),

/***/ 71342:
/*!*******************************************************!*\
  !*** ../../packages/p5-kernel-extension/lib/index.js ***!
  \*******************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/coreutils */ 78548);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlite/kernel */ 64831);
/* harmony import */ var _jupyterlite_p5_kernel__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @jupyterlite/p5-kernel */ 17489);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.



/**
 * The default CDN fallback for p5.js
 */
const P5_CDN_URL = 'https://cdn.jsdelivr.net/npm/p5@1.3.1/lib/p5.js';
/**
 * A plugin to register the p5.js kernel.
 */
const kernel = {
    id: '@jupyterlite/p5-kernel-extension:kernel',
    autoStart: true,
    requires: [_jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_1__.IKernelSpecs],
    activate: (app, kernelspecs) => {
        const url = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PageConfig.getOption('p5Url') || P5_CDN_URL;
        const p5Url = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.isLocal(url) ? _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(window.location.origin, url) : url;
        kernelspecs.register({
            spec: {
                name: 'p5js',
                display_name: 'p5.js',
                language: 'javascript',
                argv: [],
                spec: {
                    argv: [],
                    env: {},
                    display_name: 'p5.js',
                    language: 'javascript',
                    interrupt_mode: 'message',
                    metadata: {}
                },
                resources: {
                    'logo-32x32': 'TODO',
                    'logo-64x64': '/kernelspecs/p5js.png'
                }
            },
            create: async (options) => {
                return new _jupyterlite_p5_kernel__WEBPACK_IMPORTED_MODULE_2__.P5Kernel({
                    ...options,
                    p5Url
                });
            }
        });
    }
};
const plugins = [kernel];
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (plugins);


/***/ }),

/***/ 17489:
/*!**********************************************!*\
  !*** ../../packages/p5-kernel/lib/kernel.js ***!
  \**********************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "P5Kernel": () => (/* binding */ P5Kernel)
/* harmony export */ });
/* harmony import */ var _jupyterlite_javascript_kernel__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlite/javascript-kernel */ 30810);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/coreutils */ 66065);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__);


/**
 * The mimetype for mime bundle results
 */
const MIME_TYPE = 'text/html-sandboxed';
/**
 * A kernel that executes code in an IFrame.
 */
class P5Kernel extends _jupyterlite_javascript_kernel__WEBPACK_IMPORTED_MODULE_1__.JavaScriptKernel {
    /**
     * Instantiate a new P5Kernel.
     *
     * @param options The instantiation options for a new P5Kernel.
     */
    constructor(options) {
        super(options);
        this._bootstrap = '';
        this._inputs = [];
        this._p5Ready = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.PromiseDelegate();
        const { p5Url } = options;
        this._bootstrap = `
      import('${p5Url}').then(() => {
        // create the p5 global instance
        window.__globalP5 = new p5();
        return Promise.resolve();
      })
    `;
        // wait for the parent IFrame to be ready
        super.ready.then(() => {
            this._eval(this._bootstrap);
            this._p5Ready.resolve();
        });
    }
    /**
     * A promise that is fulfilled when the kernel is ready.
     */
    get ready() {
        return this._p5Ready.promise;
    }
    /**
     * Handle a kernel_info_request message
     */
    async kernelInfoRequest() {
        const content = {
            implementation: 'p5.js',
            implementation_version: '0.1.0',
            language_info: {
                codemirror_mode: {
                    name: 'javascript'
                },
                file_extension: '.js',
                mimetype: 'text/javascript',
                name: 'p5js',
                nbconvert_exporter: 'javascript',
                pygments_lexer: 'javascript',
                version: 'es2017'
            },
            protocol_version: '5.3',
            status: 'ok',
            banner: 'A p5.js kernel',
            help_links: [
                {
                    text: 'p5.js Kernel',
                    url: 'https://github.com/jupyterlite/jupyterlite'
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
        const { code } = content;
        if (code.startsWith('%')) {
            const res = await this._magics(code);
            if (res) {
                this.publishExecuteResult(res);
                return {
                    status: 'ok',
                    execution_count: this.executionCount,
                    user_expressions: {}
                };
            }
        }
        const res = super.executeRequest(content);
        this._inputs.push(code);
        return res;
    }
    /**
     * Handle magics coming from execute requests.
     *
     * @param code The code block to handle.
     */
    async _magics(code) {
        var _a, _b;
        if (code.startsWith('%show')) {
            const input = this._inputs.map(c => `window.eval(\`${c}\`);`).join('\n');
            const script = `
        ${this._bootstrap}.then(() => {
          ${input}
          window.__globalP5._start();
        })
      `;
            // add metadata
            const re = /^%show(?: (.+)\s+(.+))?\s*$/;
            const matches = code.match(re);
            const width = (_a = matches === null || matches === void 0 ? void 0 : matches[1]) !== null && _a !== void 0 ? _a : undefined;
            const height = (_b = matches === null || matches === void 0 ? void 0 : matches[2]) !== null && _b !== void 0 ? _b : undefined;
            return {
                execution_count: this.executionCount,
                data: {
                    [MIME_TYPE]: [
                        '<body style="overflow: hidden;">',
                        `<script>${script}</script>`,
                        '</body>'
                    ].join('\n')
                },
                metadata: {
                    [MIME_TYPE]: {
                        width,
                        height
                    }
                }
            };
        }
    }
}


/***/ })

}]);
//# sourceMappingURL=packages_p5-kernel-extension_lib_index_js.34d3e7b366fae2d32450.js.map