(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["packages_kernel_lib_kernel_js-packages_kernel_lib_tokens_js"],{

/***/ 65246:
/*!*******************************************!*\
  !*** ../../packages/kernel/lib/kernel.js ***!
  \*******************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "BaseKernel": () => (/* binding */ BaseKernel)
/* harmony export */ });
/* harmony import */ var _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/services */ 75357);
/* harmony import */ var _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _lumino_signaling__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @lumino/signaling */ 58137);
/* harmony import */ var _lumino_signaling__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_lumino_signaling__WEBPACK_IMPORTED_MODULE_1__);


/**
 * A base kernel class handling basic kernel messaging.
 */
class BaseKernel {
    /**
     * Construct a new BaseKernel.
     *
     * @param options The instantiation options for a BaseKernel.
     */
    constructor(options) {
        this._history = [];
        this._executionCount = 0;
        this._isDisposed = false;
        this._disposed = new _lumino_signaling__WEBPACK_IMPORTED_MODULE_1__.Signal(this);
        this._parentHeader = undefined;
        this._parent = undefined;
        const { id, name, sendMessage } = options;
        this._id = id;
        this._name = name;
        this._sendMessage = sendMessage;
    }
    /**
     * A promise that is fulfilled when the kernel is ready.
     */
    get ready() {
        return Promise.resolve();
    }
    /**
     * Return whether the kernel is disposed.
     */
    get isDisposed() {
        return this._isDisposed;
    }
    /**
     * A signal emitted when the kernel is disposed.
     */
    get disposed() {
        return this._disposed;
    }
    /**
     * Get the kernel id
     */
    get id() {
        return this._id;
    }
    /**
     * Get the name of the kernel
     */
    get name() {
        return this._name;
    }
    /**
     * The current execution count
     */
    get executionCount() {
        return this._executionCount;
    }
    /**
     * Get the last parent header
     */
    get parentHeader() {
        return this._parentHeader;
    }
    /**
     * Get the last parent message (mimick ipykernel's get_parent)
     */
    get parent() {
        return this._parent;
    }
    /**
     * Dispose the kernel.
     */
    dispose() {
        if (this.isDisposed) {
            return;
        }
        this._isDisposed = true;
        this._disposed.emit(void 0);
    }
    /**
     * Handle an incoming message from the client.
     *
     * @param msg The message to handle
     */
    async handleMessage(msg) {
        this._busy(msg);
        this._parent = msg;
        const msgType = msg.header.msg_type;
        switch (msgType) {
            case 'kernel_info_request':
                await this._kernelInfo(msg);
                break;
            case 'execute_request':
                await this._execute(msg);
                break;
            case 'input_reply':
                this.inputReply(msg.content);
                break;
            case 'inspect_request':
                await this._inspect(msg);
                break;
            case 'is_complete_request':
                await this._isCompleteRequest(msg);
                break;
            case 'complete_request':
                await this._complete(msg);
                break;
            case 'history_request':
                await this._historyRequest(msg);
                break;
            case 'comm_open':
                await this.commOpen(msg);
                break;
            case 'comm_msg':
                await this.commMsg(msg);
                break;
            case 'comm_close':
                await this.commClose(msg);
                break;
            default:
                break;
        }
        this._idle(msg);
    }
    /**
     * Stream an event from the kernel
     *
     * @param parentHeader The parent header.
     * @param content The stream content.
     */
    stream(content) {
        var _a, _b;
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            channel: 'iopub',
            msgType: 'stream',
            // TODO: better handle this
            session: (_b = (_a = this._parentHeader) === null || _a === void 0 ? void 0 : _a.session) !== null && _b !== void 0 ? _b : '',
            parentHeader: this._parentHeader,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Send a `display_data` message to the client.
     *
     * @param content The display_data content.
     */
    displayData(content) {
        var _a, _b, _c;
        // Make sure metadata is always set
        content.metadata = (_a = content.metadata) !== null && _a !== void 0 ? _a : {};
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            channel: 'iopub',
            msgType: 'display_data',
            // TODO: better handle this
            session: (_c = (_b = this._parentHeader) === null || _b === void 0 ? void 0 : _b.session) !== null && _c !== void 0 ? _c : '',
            parentHeader: this._parentHeader,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Send a `input_request` message to the client.
     *
     * @param content The input_request content.
     */
    inputRequest(content) {
        var _a, _b;
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            channel: 'stdin',
            msgType: 'input_request',
            // TODO: better handle this
            session: (_b = (_a = this._parentHeader) === null || _a === void 0 ? void 0 : _a.session) !== null && _b !== void 0 ? _b : '',
            parentHeader: this._parentHeader,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Send an `execute_result` message.
     *
     * @param content The execut result content.
     */
    publishExecuteResult(content) {
        var _a, _b;
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            channel: 'iopub',
            msgType: 'execute_result',
            // TODO: better handle this
            session: (_b = (_a = this._parentHeader) === null || _a === void 0 ? void 0 : _a.session) !== null && _b !== void 0 ? _b : '',
            parentHeader: this._parentHeader,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Send an `error` message to the client.
     *
     * @param content The error content.
     */
    publishExecuteError(content) {
        var _a, _b;
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            channel: 'iopub',
            msgType: 'error',
            // TODO: better handle this
            session: (_b = (_a = this._parentHeader) === null || _a === void 0 ? void 0 : _a.session) !== null && _b !== void 0 ? _b : '',
            parentHeader: this._parentHeader,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Send a `update_display_data` message to the client.
     *
     * @param content The update_display_data content.
     */
    updateDisplayData(content) {
        var _a, _b;
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            channel: 'iopub',
            msgType: 'update_display_data',
            // TODO: better handle this
            session: (_b = (_a = this._parentHeader) === null || _a === void 0 ? void 0 : _a.session) !== null && _b !== void 0 ? _b : '',
            parentHeader: this._parentHeader,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Send a `clear_output` message to the client.
     *
     * @param content The clear_output content.
     */
    clearOutput(content) {
        var _a, _b;
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            channel: 'iopub',
            msgType: 'clear_output',
            // TODO: better handle this
            session: (_b = (_a = this._parentHeader) === null || _a === void 0 ? void 0 : _a.session) !== null && _b !== void 0 ? _b : '',
            parentHeader: this._parentHeader,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Send a `comm` message to the client.
     *
     * @param .
     */
    handleComm(type, content, metadata, buffers) {
        var _a, _b;
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            channel: 'iopub',
            msgType: type,
            // TODO: better handle this
            session: (_b = (_a = this._parentHeader) === null || _a === void 0 ? void 0 : _a.session) !== null && _b !== void 0 ? _b : '',
            parentHeader: this._parentHeader,
            content,
            metadata,
            buffers
        });
        this._sendMessage(message);
    }
    /**
     * Send an 'idle' status message.
     *
     * @param parent The parent message
     */
    _idle(parent) {
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            msgType: 'status',
            session: parent.header.session,
            parentHeader: parent.header,
            channel: 'iopub',
            content: {
                execution_state: 'idle'
            }
        });
        this._sendMessage(message);
    }
    /**
     * Send a 'busy' status message.
     *
     * @param parent The parent message.
     */
    _busy(parent) {
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            msgType: 'status',
            session: parent.header.session,
            parentHeader: parent.header,
            channel: 'iopub',
            content: {
                execution_state: 'busy'
            }
        });
        this._sendMessage(message);
    }
    /**
     * Handle a kernel_info_request message
     *
     * @param parent The parent message.
     */
    async _kernelInfo(parent) {
        const content = await this.kernelInfoRequest();
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            msgType: 'kernel_info_reply',
            channel: 'shell',
            session: parent.header.session,
            parentHeader: parent.header,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Handle a `history_request` message
     *
     * @param msg The parent message.
     */
    async _historyRequest(msg) {
        const historyMsg = msg;
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            msgType: 'history_reply',
            channel: 'shell',
            parentHeader: historyMsg.header,
            session: msg.header.session,
            content: {
                status: 'ok',
                history: this._history
            }
        });
        this._sendMessage(message);
    }
    /**
     * Send an `execute_input` message.
     *
     * @param msg The parent message.
     */
    _executeInput(msg) {
        const parent = msg;
        const code = parent.content.code;
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            msgType: 'execute_input',
            parentHeader: parent.header,
            channel: 'iopub',
            session: msg.header.session,
            content: {
                code,
                execution_count: this._executionCount
            }
        });
        this._sendMessage(message);
    }
    /**
     * Handle an execute_request message.
     *
     * @param msg The parent message.
     */
    async _execute(msg) {
        const executeMsg = msg;
        const content = executeMsg.content;
        this._executionCount++;
        // TODO: handle differently
        this._parentHeader = executeMsg.header;
        this._executeInput(executeMsg);
        this._history.push([0, 0, content.code]);
        const reply = await this.executeRequest(executeMsg.content);
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            msgType: 'execute_reply',
            channel: 'shell',
            parentHeader: executeMsg.header,
            session: msg.header.session,
            content: reply
        });
        this._sendMessage(message);
    }
    /**
     * Handle an complete_request message
     *
     * @param msg The parent message.
     */
    async _complete(msg) {
        const completeMsg = msg;
        const content = await this.completeRequest(completeMsg.content);
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            msgType: 'complete_reply',
            parentHeader: completeMsg.header,
            channel: 'shell',
            session: msg.header.session,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Handle an inspect_request message
     *
     * @param msg The parent message.
     */
    async _inspect(msg) {
        const inspectMsg = msg;
        const content = await this.inspectRequest(inspectMsg.content);
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            msgType: 'inspect_reply',
            parentHeader: inspectMsg.header,
            channel: 'shell',
            session: msg.header.session,
            content
        });
        this._sendMessage(message);
    }
    /**
     * Handle an is_complete_request message
     *
     * @param msg The parent message.
     */
    async _isCompleteRequest(msg) {
        const isCompleteMsg = msg;
        const content = await this.isCompleteRequest(isCompleteMsg.content);
        const message = _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.KernelMessage.createMessage({
            msgType: 'is_complete_reply',
            parentHeader: isCompleteMsg.header,
            channel: 'shell',
            session: msg.header.session,
            content
        });
        this._sendMessage(message);
    }
}


/***/ }),

/***/ 64831:
/*!*******************************************!*\
  !*** ../../packages/kernel/lib/tokens.js ***!
  \*******************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "IKernels": () => (/* binding */ IKernels),
/* harmony export */   "IKernelSpecs": () => (/* binding */ IKernelSpecs)
/* harmony export */ });
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/coreutils */ 66065);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__);

/**
 * The token for the kernels service.
 */
const IKernels = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.Token('@jupyterlite/kernel:IKernels');
/**
 * The token for the kernel spec service.
 */
const IKernelSpecs = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.Token('@jupyterlite/kernelspec:IKernelSpecs');


/***/ })

}]);
//# sourceMappingURL=packages_kernel_lib_kernel_js-packages_kernel_lib_tokens_js.1718da72f6853fd3261e.js.map