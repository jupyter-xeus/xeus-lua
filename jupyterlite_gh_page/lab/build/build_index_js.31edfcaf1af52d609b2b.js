(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["build_index_js"],{

/***/ 60880:
/*!************************!*\
  !*** ./build/index.js ***!
  \************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var _jupyterlab_application__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/application */ 67599);
/* harmony import */ var _jupyterlab_application__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_application__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlite_server__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlite/server */ 52075);
/* harmony import */ var _jupyterlite_server__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_jupyterlite_server__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @jupyterlab/coreutils */ 78548);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_2__);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.





// The webpack public path needs to be set before loading the CSS assets.


const styles = Promise.all(/*! import() */[__webpack_require__.e("vendors-node_modules_css-loader_dist_runtime_api_js-node_modules_css-loader_dist_runtime_cssW-926fd9"), __webpack_require__.e("vendors-node_modules_jupyterlab_application-extension_style_index_js-node_modules_jupyterlab_-5db142"), __webpack_require__.e("build_style_js")]).then(__webpack_require__.bind(__webpack_require__, /*! ./style.js */ 4584));

const serverExtensions = [
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_javascript-kernel-extension_jupyterlite_javascrip-1fe12b").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/javascript-kernel-extension */ 89595, 23)),
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_p5-kernel-extension_jupyterlite_p5-kernel-extension").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/p5-kernel-extension */ 93337, 23)),
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_pyolite-kernel-extension_jupyterlite_pyolite-kern-46f9fb").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/pyolite-kernel-extension */ 85260, 23)),
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_xeus-kernel-extension_jupyterlite_xeus-kernel-extension").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/xeus-kernel-extension */ 54959, 23)),
  __webpack_require__.e(/*! import() */ "webpack_sharing_consume_default_jupyterlite_server-extension_jupyterlite_server-extension").then(__webpack_require__.t.bind(__webpack_require__, /*! @jupyterlite/server-extension */ 31440, 23))
];

// custom list of disabled plugins
const disabled = [
  ...JSON.parse(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_2__.PageConfig.getOption('disabledExtensions') || '[]'),
  '@jupyterlab/apputils-extension:workspaces',
  '@jupyterlab/application-extension:logo',
  '@jupyterlab/application-extension:main',
  '@jupyterlab/application-extension:tree-resolver',
  '@jupyterlab/apputils-extension:resolver',
  '@jupyterlab/docmanager-extension:download',
  '@jupyterlab/filebrowser-extension:download',
  '@jupyterlab/help-extension:about'
];

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
  // Make sure the styles have loaded
  await styles;

  const pluginsToRegister = [];
  const federatedExtensionPromises = [];
  const federatedMimeExtensionPromises = [];
  const federatedStylePromises = [];

  // This is all the data needed to load and activate plugins. This should be
  // gathered by the server and put onto the initial page template.
  const extensions = JSON.parse(
    _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_2__.PageConfig.getOption('federated_extensions')
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

  // Handle the mime extensions.
  const mimeExtensions = [];
  if (!federatedExtensionNames.has('@jupyterlite/iframe-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlite/iframe-extension */ 57968);
      for (let plugin of activePlugins(ext)) {
        mimeExtensions.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/json-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/json-extension */ 65307);
      for (let plugin of activePlugins(ext)) {
        mimeExtensions.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/vega5-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/vega5-extension */ 74425);
      for (let plugin of activePlugins(ext)) {
        mimeExtensions.push(plugin);
      }
    } catch (e) {
      console.error(e);
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

  // Handled the standard extensions.
  if (!federatedExtensionNames.has('@jupyterlite/application-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlite/application-extension */ 15881);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/application-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/application-extension */ 56953);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/apputils-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/apputils-extension */ 90426);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/celltags-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/celltags-extension */ 98475);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/codemirror-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/codemirror-extension */ 81342);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/completer-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/completer-extension */ 51563);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/console-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/console-extension */ 17161);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/csvviewer-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/csvviewer-extension */ 43277);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/docmanager-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/docmanager-extension */ 68979);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/filebrowser-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/filebrowser-extension */ 23521);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/fileeditor-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/fileeditor-extension */ 63837);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/help-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/help-extension */ 31659);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/imageviewer-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/imageviewer-extension */ 77259);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/inspector-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/inspector-extension */ 6907);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/launcher-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/launcher-extension */ 78329);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/logconsole-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/logconsole-extension */ 41832);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/mainmenu-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/mainmenu-extension */ 94253);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/markdownviewer-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/markdownviewer-extension */ 87573);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/mathjax2-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/mathjax2-extension */ 44724);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/notebook-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/notebook-extension */ 89694);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/rendermime-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/rendermime-extension */ 90);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/running-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/running-extension */ 20954);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/settingeditor-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/settingeditor-extension */ 50163);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/shortcuts-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/shortcuts-extension */ 77471);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/statusbar-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/statusbar-extension */ 16321);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/theme-dark-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/theme-dark-extension */ 18808);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/theme-light-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/theme-light-extension */ 71425);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/toc-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/toc-extension */ 87585);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/tooltip-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/tooltip-extension */ 44508);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }
  if (!federatedExtensionNames.has('@jupyterlab/ui-components-extension')) {
    try {
      let ext = __webpack_require__(/*! @jupyterlab/ui-components-extension */ 1979);
      for (let plugin of activePlugins(ext)) {
        pluginsToRegister.push(plugin);
      }
    } catch (e) {
      console.error(e);
    }
  }

  // Add the federated extensions.
  const federatedExtensions = await Promise.allSettled(federatedExtensionPromises);
  federatedExtensions.forEach(p => {
    if (p.status === "fulfilled") {
      for (let plugin of activePlugins(p.value)) {
        pluginsToRegister.push(plugin);
      }
    } else {
      console.error(p.reason);
    }
  });

  // Load all federated component styles and log errors for any that do not
  (await Promise.allSettled(federatedStylePromises)).filter(({status}) => status === "rejected").forEach(({reason}) => {
     console.error(reason);
    });

  // create the in-browser JupyterLite Server
  const jupyterLiteServer = new _jupyterlite_server__WEBPACK_IMPORTED_MODULE_1__.JupyterLiteServer({});
  jupyterLiteServer.registerPluginModules(await Promise.all(serverExtensions));
  // start the server
  await jupyterLiteServer.start();

  // retrieve the custom service manager from the server app
  const { serviceManager } = jupyterLiteServer;

  // create a full-blown JupyterLab frontend
  const lab = new _jupyterlab_application__WEBPACK_IMPORTED_MODULE_0__.JupyterLab({
    mimeExtensions,
    serviceManager
  });
  lab.name = 'JupyterLite';

  lab.registerPluginModules(pluginsToRegister);

  /* eslint-disable no-console */
  console.log('Starting app');
  await lab.start();
  console.log(`${lab.name} started, waiting for restore`);
  await lab.restored;
  console.log(`${lab.name} restored`);
}

main();


/***/ })

}]);
//# sourceMappingURL=build_index_js.31edfcaf1af52d609b2b.js.map