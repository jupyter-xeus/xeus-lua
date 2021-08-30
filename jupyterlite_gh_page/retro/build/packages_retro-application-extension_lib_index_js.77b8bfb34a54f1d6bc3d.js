(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["packages_retro-application-extension_lib_index_js"],{

/***/ 90486:
/*!***************************************************************!*\
  !*** ../../packages/retro-application-extension/lib/index.js ***!
  \***************************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _jupyterlab_application__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/application */ 67599);
/* harmony import */ var _jupyterlab_application__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_application__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlab/coreutils */ 78548);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var _jupyterlab_docmanager__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @jupyterlab/docmanager */ 60626);
/* harmony import */ var _jupyterlab_docmanager__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_docmanager__WEBPACK_IMPORTED_MODULE_2__);
/* harmony import */ var _jupyterlite_ui_components__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! @jupyterlite/ui-components */ 41415);
/* harmony import */ var _jupyterlite_ui_components__WEBPACK_IMPORTED_MODULE_3___default = /*#__PURE__*/__webpack_require__.n(_jupyterlite_ui_components__WEBPACK_IMPORTED_MODULE_3__);
/* harmony import */ var _lumino_widgets__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! @lumino/widgets */ 77031);
/* harmony import */ var _lumino_widgets__WEBPACK_IMPORTED_MODULE_4___default = /*#__PURE__*/__webpack_require__.n(_lumino_widgets__WEBPACK_IMPORTED_MODULE_4__);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.





/**
 * The default notebook factory.
 */
const NOTEBOOK_FACTORY = 'Notebook';
/**
 * The editor factory.
 */
const EDITOR_FACTORY = 'Editor';
/**
 * A regular expression to match path to notebooks and documents
 */
const TREE_PATTERN = new RegExp('/(notebooks|edit)\\/?');
/**
 * A plugin to open document in a new browser tab.
 *
 * TODO: remove and use a custom doc manager?
 */
const docmanager = {
    id: '@jupyterlite/retro-application-extension:docmanager',
    requires: [_jupyterlab_docmanager__WEBPACK_IMPORTED_MODULE_2__.IDocumentManager],
    autoStart: true,
    activate: (app, docManager) => {
        const baseUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__.PageConfig.getBaseUrl();
        // patch the `docManager.open` option to prevent the default behavior
        const docOpen = docManager.open;
        docManager.open = (path, widgetName = 'default', kernel, options) => {
            const ref = options === null || options === void 0 ? void 0 : options.ref;
            if (ref === '_noref') {
                docOpen.call(docManager, path, widgetName, kernel, options);
                return;
            }
            const ext = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__.PathExt.extname(path);
            const route = ext === '.ipynb' ? 'notebooks' : 'edit';
            window.open(`${baseUrl}retro/${route}?path=${path}`);
            return undefined;
        };
    }
};
/**
 * The logo plugin.
 */
const logo = {
    id: '@jupyterlite/retro-application-extension:logo',
    autoStart: true,
    activate: (app) => {
        const baseUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__.PageConfig.getBaseUrl();
        const node = document.createElement('a');
        node.href = `${baseUrl}retro/tree`;
        node.target = '_blank';
        node.rel = 'noopener noreferrer';
        const logo = new _lumino_widgets__WEBPACK_IMPORTED_MODULE_4__.Widget({ node });
        _jupyterlite_ui_components__WEBPACK_IMPORTED_MODULE_3__.liteWordmark.element({
            container: node,
            elementPosition: 'center',
            padding: '2px 2px 2px 8px',
            height: '28px',
            width: 'auto'
        });
        logo.id = 'jp-RetroLogo';
        app.shell.add(logo, 'top', { rank: 0 });
    }
};
/**
 * A custom openeer plugin to pass the path to documents as
 * query string parameters.
 */
const opener = {
    id: '@jupyterlite/retro-application-extension:opener',
    autoStart: true,
    requires: [_jupyterlab_application__WEBPACK_IMPORTED_MODULE_0__.IRouter, _jupyterlab_docmanager__WEBPACK_IMPORTED_MODULE_2__.IDocumentManager],
    activate: (app, router, docManager) => {
        const { commands } = app;
        const command = 'router:tree';
        commands.addCommand(command, {
            execute: (args) => {
                var _a;
                const parsed = args;
                // use request to do the matching
                const matches = (_a = parsed.request.match(TREE_PATTERN)) !== null && _a !== void 0 ? _a : [];
                if (!matches) {
                    return;
                }
                const urlParams = new URLSearchParams(window.location.search);
                const path = urlParams.get('path');
                if (!path) {
                    return;
                }
                const file = decodeURIComponent(path);
                const ext = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__.PathExt.extname(file);
                app.restored.then(() => {
                    // TODO: get factory from file type instead?
                    if (ext === '.ipynb') {
                        docManager.open(file, NOTEBOOK_FACTORY, undefined, {
                            ref: '_noref'
                        });
                    }
                    else {
                        docManager.open(file, EDITOR_FACTORY, undefined, {
                            ref: '_noref'
                        });
                    }
                });
            }
        });
        router.register({ command, pattern: TREE_PATTERN });
    }
};
const plugins = [docmanager, logo, opener];
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (plugins);


/***/ })

}]);
//# sourceMappingURL=packages_retro-application-extension_lib_index_js.77b8bfb34a54f1d6bc3d.js.map