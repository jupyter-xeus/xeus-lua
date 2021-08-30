(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["build_index_js"],{

/***/ 60880:
/*!************************!*\
  !*** ./build/index.js ***!
  \************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var _jupyterlite_server__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlite/server */ 52075);
/* harmony import */ var _jupyterlite_server__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlite_server__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlab/coreutils */ 34349);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.



// The webpack public path needs to be set before loading the CSS assets.


__webpack_require__(/*! ./style.js */ 4584);

const serverMods = [
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_javascript-kernel-extension_jupyterlite_javascrip-1fe12b").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/javascript-kernel-extension */ 89595, 23)),
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_p5-kernel-extension_jupyterlite_p5-kernel-extension").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/p5-kernel-extension */ 93337, 23)),
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_pyolite-kernel-extension_jupyterlite_pyolite-kern-46f9fb").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/pyolite-kernel-extension */ 85260, 23)),
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_xeus-kernel-extension").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/xeus-kernel-extension */ 27270, 23)),
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_server-extension_jupyterlite_server-extension").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/server-extension */ 31440, 23))
];

const mimeExtensionsMods = [
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_iframe-extension_jupyterlite_iframe-extension").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/iframe-extension */ 57968, 23)),
  Promise.all(/*! import() */[__webpack_require__.e("vendors-node_modules_prop-types_index_js"), __webpack_require__.e("vendors-node_modules_jupyterlab_json-extension_lib_index_js"), __webpack_require__.e("webpack_sharing_consume_default_lumino_coreutils_lumino_coreutils"), __webpack_require__.e("webpack_sharing_consume_default_lumino_widgets_lumino_widgets"), __webpack_require__.e("webpack_sharing_consume_default_jupyterlab_apputils_jupyterlab_apputils"), __webpack_require__.e("webpack_sharing_consume_default_jupyterlab_translation_jupyterlab_translation"), __webpack_require__.e("webpack_sharing_consume_default_jupyterlab_ui-components_jupyterlab_ui-components"), __webpack_require__.e("webpack_sharing_consume_default_react_react"), __webpack_require__.e("webpack_sharing_consume_default_react-dom_react-dom")]).then(__webpack_require__.bind(__webpack_require__, /*! @jupyterlab/json-extension */ 58863)),
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlab_vega5-extension_jupyterlab_vega5-extension").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlab/vega5-extension */ 74425, 23))
];

const disabled = JSON.parse(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__.PageConfig.getOption('disabledExtensions') || '[]');

async function createModule(scope, module) {
  try {
    const factory = await window._JUPYTERLAB[scope].get(module);
    return factory();
  } catch (e) {
    console.warn(`Failed to create module: package: ${scope}; module: ${module}`);
    throw e;
  }
}

/**
 * The main entry point for the application.
 */
async function main() {
  // create the in-browser JupyterLite Server
  const jupyterLiteServer = new _jupyterlite_server__WEBPACK_IMPORTED_MODULE_0__.JupyterLiteServer({});
  jupyterLiteServer.registerPluginModules(await Promise.all(serverMods));
  // start the server
  await jupyterLiteServer.start();

  // retrieve the custom service manager from the server app
  const { serviceManager } = jupyterLiteServer;

  const mimeExtensions = await Promise.all(mimeExtensionsMods);

  let mods = [
    // @jupyterlite plugins
    __webpack_require__(/*! @jupyterlite/application-extension */ 15881),
    __webpack_require__(/*! @jupyterlite/retro-application-extension */ 91974),
    // @retrolab plugins
    // do not enable the document opener from RetroLab
    __webpack_require__(/*! @retrolab/application-extension */ 75733).default.filter(
      ({ id }) => ![
        '@retrolab/application-extension:logo',
        '@retrolab/application-extension:opener'
      ].includes(id)
    ),
    __webpack_require__(/*! @retrolab/help-extension */ 81055),
    __webpack_require__(/*! @retrolab/notebook-extension */ 24051),

    // @jupyterlab plugins
    __webpack_require__(/*! @jupyterlab/application-extension */ 56953).default.filter(({ id }) =>
      [
        '@jupyterlab/application-extension:commands',
        '@jupyterlab/application-extension:context-menu',
        '@jupyterlab/application-extension:faviconbusy'
      ].includes(id)
    ),
    __webpack_require__(/*! @jupyterlab/apputils-extension */ 90426).default.filter(({ id }) =>
      [
        '@jupyterlab/apputils-extension:palette',
        '@jupyterlab/apputils-extension:settings',
        '@jupyterlab/apputils-extension:state',
        '@jupyterlab/apputils-extension:themes',
        '@jupyterlab/apputils-extension:themes-palette-menu'
      ].includes(id)
    ),
    __webpack_require__(/*! @jupyterlab/codemirror-extension */ 81342).default.filter(({ id }) =>
      [
        '@jupyterlab/codemirror-extension:services',
        '@jupyterlab/codemirror-extension:codemirror'
      ].includes(id)
    ),
    __webpack_require__(/*! @jupyterlab/completer-extension */ 51563).default.filter(({ id }) =>
      ['@jupyterlab/completer-extension:manager'].includes(id)
    ),
    __webpack_require__(/*! @jupyterlab/docmanager-extension */ 68979).default.filter(({ id }) =>
      ['@jupyterlab/docmanager-extension:plugin'].includes(id)
    ),
    __webpack_require__(/*! @jupyterlab/mainmenu-extension */ 94253),
    __webpack_require__(/*! @jupyterlab/mathjax2-extension */ 44724),
    __webpack_require__(/*! @jupyterlab/notebook-extension */ 89694).default.filter(({ id }) =>
      [
        '@jupyterlab/notebook-extension:factory',
        '@jupyterlab/notebook-extension:tracker',
        '@jupyterlab/notebook-extension:widget-factory'
      ].includes(id)
    ),
    __webpack_require__(/*! @jupyterlab/rendermime-extension */ 90),
    __webpack_require__(/*! @jupyterlab/shortcuts-extension */ 77471),
    __webpack_require__(/*! @jupyterlab/theme-light-extension */ 71425),
    __webpack_require__(/*! @jupyterlab/theme-dark-extension */ 18808)
  ];

  // The motivation here is to only load a specific set of plugins dependending on
  // the current page
  const page = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__.PageConfig.getOption('retroPage');
  switch (page) {
    case 'tree': {
      mods = mods.concat([
        __webpack_require__(/*! @jupyterlab/filebrowser-extension */ 23521).default.filter(({ id }) =>
          [
            '@jupyterlab/filebrowser-extension:browser',
            '@jupyterlab/filebrowser-extension:factory',
            '@jupyterlab/filebrowser-extension:file-upload-status',
            '@jupyterlab/filebrowser-extension:open-with',
            '@jupyterlab/filebrowser-extension:share-file'
          ].includes(id)
        ),
        // do not enable the new terminal button from RetroLab
        __webpack_require__(/*! @retrolab/tree-extension */ 27375).default.filter(
          ({ id }) => id !== '@retrolab/tree-extension:new-terminal'
        )
      ]);
      break;
    }
    case 'notebooks': {
      mods = mods.concat([
        __webpack_require__(/*! @jupyterlab/completer-extension */ 51563).default.filter(({ id }) =>
          ['@jupyterlab/completer-extension:notebooks'].includes(id)
        ),
        __webpack_require__(/*! @jupyterlab/tooltip-extension */ 44508).default.filter(({ id }) =>
          [
            '@jupyterlab/tooltip-extension:manager',
            '@jupyterlab/tooltip-extension:notebooks'
          ].includes(id)
        )
      ]);
      break;
    }
    case 'edit': {
      mods = mods.concat([
        __webpack_require__(/*! @jupyterlab/completer-extension */ 51563).default.filter(({ id }) =>
          ['@jupyterlab/completer-extension:files'].includes(id)
        ),
        __webpack_require__(/*! @jupyterlab/fileeditor-extension */ 63837).default.filter(({ id }) =>
          ['@jupyterlab/fileeditor-extension:plugin'].includes(id)
        ),
        __webpack_require__(/*! @jupyterlab/filebrowser-extension */ 23521).default.filter(({ id }) =>
          [
            '@jupyterlab/filebrowser-extension:browser',
            '@jupyterlab/filebrowser-extension:factory'
          ].includes(id)
        ),
      ]);
      break;
    }
  }

  const federatedExtensionPromises = [];
  const federatedMimeExtensionPromises = [];
  const federatedStylePromises = [];

  // This is all the data needed to load and activate plugins. This should be
  // gathered by the server and put onto the initial page template.
  const extensions = JSON.parse(
    _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_1__.PageConfig.getOption('federated_extensions')
  );

  // The set of federated extension names.
  const federatedExtensionNames = new Set();

  extensions.forEach(data => {
    if (data.extension) {
      federatedExtensionNames.add(data.name);
      federatedExtensionPromises.push(createModule(data.name, data.extension));
    }
    if (data.mimeExtension) {
      federatedExtensionNames.add(data.name);
      federatedMimeExtensionPromises.push(createModule(data.name, data.mimeExtension));
    }
    if (data.style) {
      federatedStylePromises.push(createModule(data.name, data.style));
    }
  });

  /**
   * Iterate over active plugins in an extension.
   */
  function* activePlugins(extension) {
    // Handle commonjs or es2015 modules
    let exports;
    if (extension.hasOwnProperty('__esModule')) {
      exports = extension.default;
    } else {
      // CommonJS exports.
      exports = extension;
    }

    let plugins = Array.isArray(exports) ? exports : [exports];
    for (let plugin of plugins) {
      // skip the plugin (or extension) if disabled
      if (
        disabled.includes(plugin.id) ||
        disabled.includes(plugin.id.split(':')[0])
      ) {
        continue;
      }
      yield plugin;
    }
  }

  // Add the federated mime extensions.
  const federatedMimeExtensions = await Promise.allSettled(federatedMimeExtensionPromises);
  federatedMimeExtensions.forEach(p => {
    if (p.status === "fulfilled") {
      for (let plugin of activePlugins(p.value)) {
        mimeExtensions.push(plugin);
      }
    } else {
      console.error(p.reason);
    }
  });

  // Add the federated extensions.
  const federatedExtensions = await Promise.allSettled(federatedExtensionPromises);
  federatedExtensions.forEach(p => {
    if (p.status === "fulfilled") {
      for (let plugin of activePlugins(p.value)) {
        mods.push(plugin);
      }
    } else {
      console.error(p.reason);
    }
  });

  // create a RetroLab frontend
  const { RetroApp } = __webpack_require__(/*! @retrolab/application */ 97485);
  const app = new RetroApp({ serviceManager, mimeExtensions });

  app.name = 'RetroLite';

  app.registerPluginModules(mods);

  console.log('Starting app');
  await app.start();
  console.log(`${app.name} started, waiting for restore`);
  await app.restored;
  console.log(`${app.name} restored`);
}

main();


/***/ }),

/***/ 4584:
/*!************************!*\
  !*** ./build/style.js ***!
  \************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var _jupyterlab_application_extension_style_index_js__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/application-extension/style/index.js */ 30756);
/* harmony import */ var _jupyterlab_apputils_extension_style_index_js__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlab/apputils-extension/style/index.js */ 81088);
/* harmony import */ var _jupyterlab_codemirror_extension_style_index_js__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @jupyterlab/codemirror-extension/style/index.js */ 60734);
/* harmony import */ var _jupyterlab_completer_extension_style_index_js__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! @jupyterlab/completer-extension/style/index.js */ 1886);
/* harmony import */ var _jupyterlab_docmanager_extension_style_index_js__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! @jupyterlab/docmanager-extension/style/index.js */ 42413);
/* harmony import */ var _jupyterlab_filebrowser_extension_style_index_js__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! @jupyterlab/filebrowser-extension/style/index.js */ 82053);
/* harmony import */ var _jupyterlab_fileeditor_extension_style_index_js__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(/*! @jupyterlab/fileeditor-extension/style/index.js */ 10139);
/* harmony import */ var _jupyterlab_mainmenu_extension_style_index_js__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(/*! @jupyterlab/mainmenu-extension/style/index.js */ 7537);
/* harmony import */ var _jupyterlab_mathjax2_extension_style_index_js__WEBPACK_IMPORTED_MODULE_8__ = __webpack_require__(/*! @jupyterlab/mathjax2-extension/style/index.js */ 99354);
/* harmony import */ var _jupyterlab_notebook_extension_style_index_js__WEBPACK_IMPORTED_MODULE_9__ = __webpack_require__(/*! @jupyterlab/notebook-extension/style/index.js */ 94429);
/* harmony import */ var _jupyterlab_rendermime_extension_style_index_js__WEBPACK_IMPORTED_MODULE_10__ = __webpack_require__(/*! @jupyterlab/rendermime-extension/style/index.js */ 27784);
/* harmony import */ var _jupyterlab_tooltip_extension_style_index_js__WEBPACK_IMPORTED_MODULE_11__ = __webpack_require__(/*! @jupyterlab/tooltip-extension/style/index.js */ 2114);
/* harmony import */ var _jupyterlab_vega5_extension_style_index_js__WEBPACK_IMPORTED_MODULE_12__ = __webpack_require__(/*! @jupyterlab/vega5-extension/style/index.js */ 15619);
/* harmony import */ var _jupyterlite_application_extension_style_index_js__WEBPACK_IMPORTED_MODULE_13__ = __webpack_require__(/*! @jupyterlite/application-extension/style/index.js */ 38186);
/* harmony import */ var _jupyterlite_iframe_extension_style_index_js__WEBPACK_IMPORTED_MODULE_14__ = __webpack_require__(/*! @jupyterlite/iframe-extension/style/index.js */ 68510);
/* harmony import */ var _jupyterlite_retro_application_extension_style_index_js__WEBPACK_IMPORTED_MODULE_15__ = __webpack_require__(/*! @jupyterlite/retro-application-extension/style/index.js */ 31657);
/* harmony import */ var _retrolab_application_extension_style_index_js__WEBPACK_IMPORTED_MODULE_16__ = __webpack_require__(/*! @retrolab/application-extension/style/index.js */ 54101);
/* harmony import */ var _retrolab_docmanager_extension_style_index_js__WEBPACK_IMPORTED_MODULE_17__ = __webpack_require__(/*! @retrolab/docmanager-extension/style/index.js */ 20598);
/* harmony import */ var _retrolab_help_extension_style_index_js__WEBPACK_IMPORTED_MODULE_18__ = __webpack_require__(/*! @retrolab/help-extension/style/index.js */ 58489);
/* harmony import */ var _retrolab_notebook_extension_style_index_js__WEBPACK_IMPORTED_MODULE_19__ = __webpack_require__(/*! @retrolab/notebook-extension/style/index.js */ 34232);
/* harmony import */ var _retrolab_tree_extension_style_index_js__WEBPACK_IMPORTED_MODULE_20__ = __webpack_require__(/*! @retrolab/tree-extension/style/index.js */ 38527);
/* This is a generated file of CSS imports */
/* It was generated by @jupyterlab/builder in Build.ensureAssets() */
























/***/ }),

/***/ 22383:
/*!*****************************************************************************************************!*\
  !*** ../../node_modules/css-loader/dist/cjs.js!../../packages/application-extension/style/base.css ***!
  \*****************************************************************************************************/
/***/ ((module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! ../../../node_modules/css-loader/dist/runtime/cssWithMappingToString.js */ 1799);
/* harmony import */ var _node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ../../../node_modules/css-loader/dist/runtime/api.js */ 82609);
/* harmony import */ var _node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1__);
// Imports


var ___CSS_LOADER_EXPORT___ = _node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1___default()((_node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0___default()));
// Module
___CSS_LOADER_EXPORT___.push([module.id, "/*-----------------------------------------------------------------------------\n| Copyright (c) Jupyter Development Team.\n| Distributed under the terms of the Modified BSD License.\n|----------------------------------------------------------------------------*/\n", "",{"version":3,"sources":["webpack://./../../packages/application-extension/style/base.css"],"names":[],"mappings":"AAAA;;;8EAG8E","sourcesContent":["/*-----------------------------------------------------------------------------\n| Copyright (c) Jupyter Development Team.\n| Distributed under the terms of the Modified BSD License.\n|----------------------------------------------------------------------------*/\n"],"sourceRoot":""}]);
// Exports
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (___CSS_LOADER_EXPORT___);


/***/ }),

/***/ 40227:
/*!************************************************************************************************!*\
  !*** ../../node_modules/css-loader/dist/cjs.js!../../packages/iframe-extension/style/base.css ***!
  \************************************************************************************************/
/***/ ((module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! ../../../node_modules/css-loader/dist/runtime/cssWithMappingToString.js */ 1799);
/* harmony import */ var _node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ../../../node_modules/css-loader/dist/runtime/api.js */ 82609);
/* harmony import */ var _node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1__);
// Imports


var ___CSS_LOADER_EXPORT___ = _node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1___default()((_node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0___default()));
// Module
___CSS_LOADER_EXPORT___.push([module.id, "/*-----------------------------------------------------------------------------\n| Copyright (c) Jupyter Development Team.\n| Distributed under the terms of the Modified BSD License.\n|----------------------------------------------------------------------------*/\n\n.jp-IFrameContainer iframe,\n.jp-IFrameContainer body {\n  margin: 0;\n  padding: 0;\n  overflow: hidden;\n  border: none;\n}\n", "",{"version":3,"sources":["webpack://./../../packages/iframe-extension/style/base.css"],"names":[],"mappings":"AAAA;;;8EAG8E;;AAE9E;;EAEE,SAAS;EACT,UAAU;EACV,gBAAgB;EAChB,YAAY;AACd","sourcesContent":["/*-----------------------------------------------------------------------------\n| Copyright (c) Jupyter Development Team.\n| Distributed under the terms of the Modified BSD License.\n|----------------------------------------------------------------------------*/\n\n.jp-IFrameContainer iframe,\n.jp-IFrameContainer body {\n  margin: 0;\n  padding: 0;\n  overflow: hidden;\n  border: none;\n}\n"],"sourceRoot":""}]);
// Exports
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (___CSS_LOADER_EXPORT___);


/***/ }),

/***/ 66633:
/*!***********************************************************************************************************!*\
  !*** ../../node_modules/css-loader/dist/cjs.js!../../packages/retro-application-extension/style/base.css ***!
  \***********************************************************************************************************/
/***/ ((module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! ../../../node_modules/css-loader/dist/runtime/cssWithMappingToString.js */ 1799);
/* harmony import */ var _node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ../../../node_modules/css-loader/dist/runtime/api.js */ 82609);
/* harmony import */ var _node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1__);
// Imports


var ___CSS_LOADER_EXPORT___ = _node_modules_css_loader_dist_runtime_api_js__WEBPACK_IMPORTED_MODULE_1___default()((_node_modules_css_loader_dist_runtime_cssWithMappingToString_js__WEBPACK_IMPORTED_MODULE_0___default()));
// Module
___CSS_LOADER_EXPORT___.push([module.id, "/*-----------------------------------------------------------------------------\n| Copyright (c) Jupyter Development Team.\n| Distributed under the terms of the Modified BSD License.\n|----------------------------------------------------------------------------*/\n", "",{"version":3,"sources":["webpack://./../../packages/retro-application-extension/style/base.css"],"names":[],"mappings":"AAAA;;;8EAG8E","sourcesContent":["/*-----------------------------------------------------------------------------\n| Copyright (c) Jupyter Development Team.\n| Distributed under the terms of the Modified BSD License.\n|----------------------------------------------------------------------------*/\n"],"sourceRoot":""}]);
// Exports
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (___CSS_LOADER_EXPORT___);


/***/ }),

/***/ 51347:
/*!***********************************************************!*\
  !*** ../../packages/application-extension/style/base.css ***!
  \***********************************************************/
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {

var content = __webpack_require__(/*! !!../../../node_modules/css-loader/dist/cjs.js!./base.css */ 22383);
content = content.__esModule ? content.default : content;

if (typeof content === 'string') {
  content = [[module.id, content, '']];
}

var options = {}

options.insert = "head";
options.singleton = false;

var update = __webpack_require__(/*! !../../../node_modules/style-loader/dist/runtime/injectStylesIntoStyleTag.js */ 46062)(content, options);

if (content.locals) {
  module.exports = content.locals;
}


/***/ }),

/***/ 34706:
/*!******************************************************!*\
  !*** ../../packages/iframe-extension/style/base.css ***!
  \******************************************************/
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {

var content = __webpack_require__(/*! !!../../../node_modules/css-loader/dist/cjs.js!./base.css */ 40227);
content = content.__esModule ? content.default : content;

if (typeof content === 'string') {
  content = [[module.id, content, '']];
}

var options = {}

options.insert = "head";
options.singleton = false;

var update = __webpack_require__(/*! !../../../node_modules/style-loader/dist/runtime/injectStylesIntoStyleTag.js */ 46062)(content, options);

if (content.locals) {
  module.exports = content.locals;
}


/***/ }),

/***/ 60717:
/*!*****************************************************************!*\
  !*** ../../packages/retro-application-extension/style/base.css ***!
  \*****************************************************************/
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {

var content = __webpack_require__(/*! !!../../../node_modules/css-loader/dist/cjs.js!./base.css */ 66633);
content = content.__esModule ? content.default : content;

if (typeof content === 'string') {
  content = [[module.id, content, '']];
}

var options = {}

options.insert = "head";
options.singleton = false;

var update = __webpack_require__(/*! !../../../node_modules/style-loader/dist/runtime/injectStylesIntoStyleTag.js */ 46062)(content, options);

if (content.locals) {
  module.exports = content.locals;
}


/***/ }),

/***/ 38186:
/*!***********************************************************!*\
  !*** ../../packages/application-extension/style/index.js ***!
  \***********************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var _jupyterlab_help_extension_style_index_js__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/help-extension/style/index.js */ 58739);
/* harmony import */ var _base_css__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ./base.css */ 51347);
/* harmony import */ var _base_css__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_base_css__WEBPACK_IMPORTED_MODULE_1__);
/*-----------------------------------------------------------------------------
| Copyright (c) Jupyter Development Team.
| Distributed under the terms of the Modified BSD License.
|----------------------------------------------------------------------------*/






/***/ }),

/***/ 68510:
/*!******************************************************!*\
  !*** ../../packages/iframe-extension/style/index.js ***!
  \******************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var _lumino_widgets_style_index_js__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/widgets/style/index.js */ 21499);
/* harmony import */ var _base_css__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ./base.css */ 34706);
/* harmony import */ var _base_css__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_base_css__WEBPACK_IMPORTED_MODULE_1__);
/*-----------------------------------------------------------------------------
| Copyright (c) Jupyter Development Team.
| Distributed under the terms of the Modified BSD License.
|----------------------------------------------------------------------------*/

/* This file was auto-generated by ensurePackage() in @jupyterlab/buildutils */





/***/ }),

/***/ 31657:
/*!*****************************************************************!*\
  !*** ../../packages/retro-application-extension/style/index.js ***!
  \*****************************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var _base_css__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! ./base.css */ 60717);
/* harmony import */ var _base_css__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_base_css__WEBPACK_IMPORTED_MODULE_0__);
/*-----------------------------------------------------------------------------
| Copyright (c) Jupyter Development Team.
| Distributed under the terms of the Modified BSD License.
|----------------------------------------------------------------------------*/




/***/ })

}]);
//# sourceMappingURL=build_index_js.adf2df3a9374891cad17.js.map