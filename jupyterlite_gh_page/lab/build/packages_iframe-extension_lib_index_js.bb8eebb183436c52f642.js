(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["packages_iframe-extension_lib_index_js"],{

/***/ 89:
/*!****************************************************!*\
  !*** ../../packages/iframe-extension/lib/index.js ***!
  \****************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "RenderedIFrame": () => (/* binding */ RenderedIFrame),
/* harmony export */   "rendererFactory": () => (/* binding */ rendererFactory),
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/coreutils */ 9727);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _lumino_widgets__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @lumino/widgets */ 61389);
/* harmony import */ var _lumino_widgets__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_lumino_widgets__WEBPACK_IMPORTED_MODULE_1__);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.


/**
 * The MIME type for IFrame.
 */
const MIME_TYPE = 'text/html-sandboxed';
/**
 * A class for rendering an IFrame document.
 */
class RenderedIFrame extends _lumino_widgets__WEBPACK_IMPORTED_MODULE_1__.Widget {
    constructor() {
        super();
        this._ready = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.PromiseDelegate();
        this.addClass('jp-IFrameContainer');
        this._iframe = document.createElement('iframe');
        // Provide default dimensions
        this._iframe.width = '100%';
        this._iframe.height = '400px';
        this._iframe.onload = () => {
            this._ready.resolve(void 0);
        };
        this.node.appendChild(this._iframe);
    }
    /**
     * Render the IFrame into this widget's node.
     */
    async renderModel(model) {
        var _a, _b;
        await this._ready.promise;
        const data = model.data[MIME_TYPE];
        if (!data || !this._iframe.contentWindow) {
            return;
        }
        const metadata = model.metadata[MIME_TYPE];
        this._iframe.width = (_a = metadata === null || metadata === void 0 ? void 0 : metadata.width) !== null && _a !== void 0 ? _a : this._iframe.width;
        this._iframe.height = (_b = metadata === null || metadata === void 0 ? void 0 : metadata.height) !== null && _b !== void 0 ? _b : this._iframe.height;
        this._iframe.contentWindow.document.write(data);
    }
    /**
     * Dispose of the resources held by the iframe widget.
     */
    dispose() {
        this._iframe.remove();
        super.dispose();
    }
}
/**
 * A mime renderer factory for IFrame data.
 */
const rendererFactory = {
    safe: false,
    mimeTypes: [MIME_TYPE],
    defaultRank: 100,
    createRenderer: options => new RenderedIFrame()
};
const extensions = [
    {
        id: '@jupyterlite/iframe-extension:factory',
        rendererFactory,
        dataType: 'string',
        documentWidgetFactoryOptions: {
            name: 'IFrame',
            primaryFileType: 'IFrame',
            fileTypes: ['IFrame'],
            defaultFor: ['IFrame']
        }
    }
];
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (extensions);


/***/ })

}]);
//# sourceMappingURL=packages_iframe-extension_lib_index_js.bb8eebb183436c52f642.js.map