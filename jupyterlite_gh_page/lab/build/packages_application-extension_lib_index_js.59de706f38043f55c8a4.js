(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["packages_application-extension_lib_index_js"],{

/***/ 6588:
/*!*********************************************************!*\
  !*** ../../packages/application-extension/lib/index.js ***!
  \*********************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _jupyterlab_application__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/application */ 67599);
/* harmony import */ var _jupyterlab_application__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_application__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlab_apputils__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlab/apputils */ 18477);
/* harmony import */ var _jupyterlab_apputils__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_apputils__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @jupyterlab/coreutils */ 78548);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_2__);
/* harmony import */ var _jupyterlab_docmanager__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! @jupyterlab/docmanager */ 60626);
/* harmony import */ var _jupyterlab_docmanager__WEBPACK_IMPORTED_MODULE_3___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_docmanager__WEBPACK_IMPORTED_MODULE_3__);
/* harmony import */ var _jupyterlab_docprovider__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! @jupyterlab/docprovider */ 5883);
/* harmony import */ var _jupyterlab_docprovider__WEBPACK_IMPORTED_MODULE_4___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_docprovider__WEBPACK_IMPORTED_MODULE_4__);
/* harmony import */ var _jupyterlab_filebrowser__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! @jupyterlab/filebrowser */ 59310);
/* harmony import */ var _jupyterlab_filebrowser__WEBPACK_IMPORTED_MODULE_5___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_filebrowser__WEBPACK_IMPORTED_MODULE_5__);
/* harmony import */ var _jupyterlab_mainmenu__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(/*! @jupyterlab/mainmenu */ 86942);
/* harmony import */ var _jupyterlab_mainmenu__WEBPACK_IMPORTED_MODULE_6___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_mainmenu__WEBPACK_IMPORTED_MODULE_6__);
/* harmony import */ var _jupyterlab_translation__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(/*! @jupyterlab/translation */ 57630);
/* harmony import */ var _jupyterlab_translation__WEBPACK_IMPORTED_MODULE_7___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_translation__WEBPACK_IMPORTED_MODULE_7__);
/* harmony import */ var _jupyterlab_ui_components__WEBPACK_IMPORTED_MODULE_8__ = __webpack_require__(/*! @jupyterlab/ui-components */ 65995);
/* harmony import */ var _jupyterlab_ui_components__WEBPACK_IMPORTED_MODULE_8___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_ui_components__WEBPACK_IMPORTED_MODULE_8__);
/* harmony import */ var _jupyterlite_ui_components__WEBPACK_IMPORTED_MODULE_9__ = __webpack_require__(/*! @jupyterlite/ui-components */ 41415);
/* harmony import */ var _jupyterlite_ui_components__WEBPACK_IMPORTED_MODULE_9___default = /*#__PURE__*/__webpack_require__.n(_jupyterlite_ui_components__WEBPACK_IMPORTED_MODULE_9__);
/* harmony import */ var _lumino_algorithm__WEBPACK_IMPORTED_MODULE_10__ = __webpack_require__(/*! @lumino/algorithm */ 62867);
/* harmony import */ var _lumino_algorithm__WEBPACK_IMPORTED_MODULE_10___default = /*#__PURE__*/__webpack_require__.n(_lumino_algorithm__WEBPACK_IMPORTED_MODULE_10__);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_11__ = __webpack_require__(/*! @lumino/coreutils */ 9727);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_11___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_11__);
/* harmony import */ var _lumino_widgets__WEBPACK_IMPORTED_MODULE_12__ = __webpack_require__(/*! @lumino/widgets */ 61389);
/* harmony import */ var _lumino_widgets__WEBPACK_IMPORTED_MODULE_12___default = /*#__PURE__*/__webpack_require__.n(_lumino_widgets__WEBPACK_IMPORTED_MODULE_12__);
/* harmony import */ var lib0_environment__WEBPACK_IMPORTED_MODULE_15__ = __webpack_require__(/*! lib0/environment */ 92588);
/* harmony import */ var y_webrtc__WEBPACK_IMPORTED_MODULE_13__ = __webpack_require__(/*! y-webrtc */ 76749);
/* harmony import */ var y_webrtc__WEBPACK_IMPORTED_MODULE_13___default = /*#__PURE__*/__webpack_require__.n(y_webrtc__WEBPACK_IMPORTED_MODULE_13__);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_14__ = __webpack_require__(/*! react */ 2411);
/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_14___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_14__);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.
















class WebRtcProvider extends y_webrtc__WEBPACK_IMPORTED_MODULE_13__.WebrtcProvider {
    constructor(options) {
        super(`${options.room}${options.path}`, options.ymodel.ydoc);
        this._initialRequest = null;
        this.awareness = options.ymodel.awareness;
        const color = `#${(0,lib0_environment__WEBPACK_IMPORTED_MODULE_15__.getParam)('--usercolor', (0,_jupyterlab_docprovider__WEBPACK_IMPORTED_MODULE_4__.getRandomColor)().slice(1))}`;
        const name = (0,lib0_environment__WEBPACK_IMPORTED_MODULE_15__.getParam)('--username', (0,_jupyterlab_docprovider__WEBPACK_IMPORTED_MODULE_4__.getAnonymousUserName)());
        const currState = this.awareness.getLocalState();
        // only set if this was not already set by another plugin
        if (currState && !currState.name) {
            this.awareness.setLocalStateField('user', {
                name,
                color
            });
        }
    }
    setPath() {
        // TODO: this seems super useful
    }
    requestInitialContent() {
        if (this._initialRequest) {
            return this._initialRequest.promise;
        }
        let resolved = false;
        this._initialRequest = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_11__.PromiseDelegate();
        this.on('synced', (event) => {
            if (this._initialRequest) {
                this._initialRequest.resolve(event.synced);
                resolved = true;
            }
        });
        // similar logic as in the upstream plugin
        setTimeout(() => {
            if (!resolved && this._initialRequest) {
                this._initialRequest.resolve(false);
            }
        }, 1000);
        return this._initialRequest.promise;
    }
    putInitializedState() {
        // no-op
    }
    acquireLock() {
        return Promise.resolve(0);
    }
    releaseLock(lock) {
        // no-op
    }
}
/**
 * The command IDs used by the application extension.
 */
var CommandIDs;
(function (CommandIDs) {
    CommandIDs.about = 'application:about';
    CommandIDs.docmanagerDownload = 'docmanager:download';
    CommandIDs.filebrowserDownload = 'filebrowser:download';
})(CommandIDs || (CommandIDs = {}));
/**
 * Add a command to show an About dialog.
 */
const about = {
    id: '@jupyterlite/application-extension:about',
    autoStart: true,
    requires: [_jupyterlab_translation__WEBPACK_IMPORTED_MODULE_7__.ITranslator],
    optional: [_jupyterlab_apputils__WEBPACK_IMPORTED_MODULE_1__.ICommandPalette, _jupyterlab_mainmenu__WEBPACK_IMPORTED_MODULE_6__.IMainMenu],
    activate: (app, translator, palette, menu) => {
        const { commands } = app;
        const trans = translator.load('jupyterlab');
        const category = trans.__('Help');
        commands.addCommand(CommandIDs.about, {
            label: trans.__('About %1', app.name),
            execute: () => {
                const versionNumber = trans.__('Version %1', app.version);
                const versionInfo = (react__WEBPACK_IMPORTED_MODULE_14___default().createElement("span", { className: "jp-About-version-info" },
                    react__WEBPACK_IMPORTED_MODULE_14___default().createElement("span", { className: "jp-About-version" }, versionNumber)));
                const title = (react__WEBPACK_IMPORTED_MODULE_14___default().createElement("span", { className: "jp-About-header" },
                    react__WEBPACK_IMPORTED_MODULE_14___default().createElement("div", { className: "jp-About-header-info" },
                        react__WEBPACK_IMPORTED_MODULE_14___default().createElement(_jupyterlite_ui_components__WEBPACK_IMPORTED_MODULE_9__.liteWordmark.react, { height: "auto", width: "196px" }),
                        versionInfo)));
                // Create the body of the about dialog
                const jupyterliteURL = 'https://github.com/jupyterlite/jupyterlite';
                const contributorsURL = 'https://github.com/jupyterlite/jupyterlite/graphs/contributors';
                const externalLinks = (react__WEBPACK_IMPORTED_MODULE_14___default().createElement("span", { className: "jp-About-externalLinks" },
                    react__WEBPACK_IMPORTED_MODULE_14___default().createElement("a", { href: contributorsURL, target: "_blank", rel: "noopener noreferrer", className: "jp-Button-flat" }, trans.__('CONTRIBUTOR LIST')),
                    react__WEBPACK_IMPORTED_MODULE_14___default().createElement("a", { href: jupyterliteURL, target: "_blank", rel: "noopener noreferrer", className: "jp-Button-flat" }, trans.__('JUPYTERLITE ON GITHUB'))));
                const copyright = (react__WEBPACK_IMPORTED_MODULE_14___default().createElement("span", { className: "jp-About-copyright" }, trans.__('© 2021 JupyterLite Contributors')));
                const body = (react__WEBPACK_IMPORTED_MODULE_14___default().createElement("div", { className: "jp-About-body" },
                    externalLinks,
                    copyright));
                return (0,_jupyterlab_apputils__WEBPACK_IMPORTED_MODULE_1__.showDialog)({
                    title,
                    body,
                    buttons: [
                        _jupyterlab_apputils__WEBPACK_IMPORTED_MODULE_1__.Dialog.createButton({
                            label: trans.__('Dismiss'),
                            className: 'jp-About-button jp-mod-reject jp-mod-styled'
                        })
                    ]
                });
            }
        });
        if (palette) {
            palette.addItem({ command: CommandIDs.about, category });
        }
        if (menu) {
            menu.helpMenu.addGroup([{ command: CommandIDs.about }], 0);
        }
    }
};
/**
 * An alternative document provider plugin
 */
const docProviderPlugin = {
    id: '@jupyterlite/application-extension:docprovider',
    provides: _jupyterlab_docprovider__WEBPACK_IMPORTED_MODULE_4__.IDocumentProviderFactory,
    activate: (app) => {
        const roomName = (0,lib0_environment__WEBPACK_IMPORTED_MODULE_15__.getParam)('--room', '').trim();
        const host = window.location.host;
        // enable if both the page config option (deployment wide) and the room name (user) are defined
        const collaborative = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_2__.PageConfig.getOption('collaborative') === 'true' && roomName;
        // default to a random id to not collaborate with others by default
        const room = `${host}-${roomName || _lumino_coreutils__WEBPACK_IMPORTED_MODULE_11__.UUID.uuid4()}`;
        const factory = (options) => {
            return collaborative
                ? new WebRtcProvider({
                    room,
                    ...options
                })
                : new _jupyterlab_docprovider__WEBPACK_IMPORTED_MODULE_4__.ProviderMock();
        };
        return factory;
    }
};
/**
 * A plugin providing download commands in the file menu and command palette.
 */
const downloadPlugin = {
    id: '@jupyterlite/application-extension:download',
    autoStart: true,
    requires: [_jupyterlab_translation__WEBPACK_IMPORTED_MODULE_7__.ITranslator, _jupyterlab_docmanager__WEBPACK_IMPORTED_MODULE_3__.IDocumentManager],
    optional: [_jupyterlab_apputils__WEBPACK_IMPORTED_MODULE_1__.ICommandPalette, _jupyterlab_filebrowser__WEBPACK_IMPORTED_MODULE_5__.IFileBrowserFactory, _jupyterlab_mainmenu__WEBPACK_IMPORTED_MODULE_6__.IMainMenu],
    activate: (app, translator, docManager, palette, factory, mainMenu) => {
        const trans = translator.load('jupyterlab');
        const { commands, contextMenu, serviceManager, shell } = app;
        const isEnabled = () => {
            const { currentWidget } = shell;
            return !!(currentWidget && docManager.contextForWidget(currentWidget));
        };
        const downloadContent = (content, path) => {
            const element = document.createElement('a');
            element.href = `data:text/json;charset=utf-8,${encodeURIComponent(content)}`;
            element.download = path;
            document.body.appendChild(element);
            element.click();
            document.body.removeChild(element);
        };
        commands.addCommand(CommandIDs.docmanagerDownload, {
            label: trans.__('Download'),
            caption: trans.__('Download the file to your computer'),
            isEnabled,
            execute: () => {
                // Checks that shell.currentWidget is valid:
                const current = shell.currentWidget;
                if (!isEnabled() || !current) {
                    return;
                }
                const context = docManager.contextForWidget(current);
                if (!context) {
                    return (0,_jupyterlab_apputils__WEBPACK_IMPORTED_MODULE_1__.showDialog)({
                        title: trans.__('Cannot Download'),
                        body: trans.__('No context found for current widget!'),
                        buttons: [_jupyterlab_apputils__WEBPACK_IMPORTED_MODULE_1__.Dialog.okButton({ label: trans.__('OK') })]
                    });
                }
                downloadContent(context.model.toString(), context.path);
            }
        });
        const category = trans.__('File Operations');
        if (palette) {
            palette.addItem({ command: CommandIDs.docmanagerDownload, category });
        }
        if (mainMenu) {
            mainMenu.fileMenu.addGroup([{ command: CommandIDs.docmanagerDownload }], 6);
        }
        if (factory) {
            const { tracker } = factory;
            const { contents } = serviceManager;
            commands.addCommand(CommandIDs.filebrowserDownload, {
                execute: async () => {
                    const widget = tracker.currentWidget;
                    if (!widget) {
                        return;
                    }
                    const selected = (0,_lumino_algorithm__WEBPACK_IMPORTED_MODULE_10__.toArray)(widget.selectedItems());
                    selected.forEach(async (item) => {
                        if (item.type === 'directory') {
                            return;
                        }
                        const file = await contents.get(item.path, { content: true });
                        const formatted = file.type === 'notebook' || file.mimetype.indexOf('json') !== -1
                            ? JSON.stringify(file.content, null, 2)
                            : file.content;
                        downloadContent(formatted, item.name);
                    });
                },
                icon: _jupyterlab_ui_components__WEBPACK_IMPORTED_MODULE_8__.downloadIcon.bindprops({ stylesheet: 'menuItem' }),
                label: trans.__('Download')
            });
            contextMenu.addItem({
                command: CommandIDs.filebrowserDownload,
                selector: '.jp-DirListing-item[data-isdir="false"]',
                rank: 9
            });
        }
    }
};
/**
 * The main application icon.
 */
const liteLogo = {
    id: '@jupyterlite/application-extension:logo',
    // marking as optional to not throw errors in retro
    optional: [_jupyterlab_application__WEBPACK_IMPORTED_MODULE_0__.ILabShell],
    autoStart: true,
    activate: (app, labShell) => {
        if (!labShell) {
            return;
        }
        const logo = new _lumino_widgets__WEBPACK_IMPORTED_MODULE_12__.Widget();
        _jupyterlite_ui_components__WEBPACK_IMPORTED_MODULE_9__.liteIcon.element({
            container: logo.node,
            elementPosition: 'center',
            margin: '2px 2px 2px 8px',
            height: 'auto',
            width: '16px'
        });
        logo.id = 'jp-MainLogo';
        labShell.add(logo, 'top', { rank: 0 });
    }
};
/**
 * A simplified Translator
 */
const translator = {
    id: '@jupyterlite/application-extension:translator',
    activate: (app) => {
        const translationManager = new _jupyterlab_translation__WEBPACK_IMPORTED_MODULE_7__.TranslationManager();
        return translationManager;
    },
    autoStart: true,
    provides: _jupyterlab_translation__WEBPACK_IMPORTED_MODULE_7__.ITranslator
};
const plugins = [
    about,
    docProviderPlugin,
    downloadPlugin,
    liteLogo,
    translator
];
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (plugins);


/***/ })

}]);
//# sourceMappingURL=packages_application-extension_lib_index_js.59de706f38043f55c8a4.js.map