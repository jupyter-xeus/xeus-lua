(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["packages_server_lib_index_js"],{

/***/ 88277:
/*!****************************************!*\
  !*** ../../packages/server/lib/app.js ***!
  \****************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "JupyterLiteServer": () => (/* binding */ JupyterLiteServer)
/* harmony export */ });
/* harmony import */ var _lumino_application__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/application */ 12131);
/* harmony import */ var _lumino_application__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_application__WEBPACK_IMPORTED_MODULE_0__);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.

/**
 * Server is the main application class. It is instantiated once and shared.
 */
class JupyterLiteServer extends _lumino_application__WEBPACK_IMPORTED_MODULE_0__.Application {
    /**
     * Construct a new JupyterLite object.
     *
     * @param options The instantiation options for a JupyterLiteServer application.
     */
    constructor(options) {
        super(options);
        /**
         * The name of the application.
         */
        this.name = 'JupyterLite Server';
        /**
         * A namespace/prefix plugins may use to denote their provenance.
         */
        this.namespace = this.name;
        /**
         * The version of the application.
         */
        this.version = 'unknown';
        this._serviceManager = null;
    }
    /**
     * Get the underlying lite service manager for this app.
     */
    get serviceManager() {
        return this._serviceManager;
    }
    /**
     * Attach the application shell to the DOM.
     *
     * @param id - The id of the host node for the shell, or `''`.
     *
     * #### Notes
     * For this server application there is no shell to attach
     */
    attachShell(id) {
        // no-op
    }
    /**
     * A method invoked on a window `'resize'` event.
     *
     * #### Notes
     * For this server application there is no shell to update
     */
    evtResize(event) {
        // no-op
    }
    /**
     * Register plugins from a plugin module.
     *
     * @param mod - The plugin module to register.
     */
    registerPluginModule(mod) {
        let data = mod.default;
        // Handle commonjs exports.
        if (!Object.prototype.hasOwnProperty.call(mod, '__esModule')) {
            data = mod;
        }
        if (!Array.isArray(data)) {
            data = [data];
        }
        data.forEach(item => {
            try {
                this.registerPlugin(item);
            }
            catch (error) {
                console.error(error);
            }
        });
    }
    /**
     * Register the plugins from multiple plugin modules.
     *
     * @param mods - The plugin modules to register.
     */
    registerPluginModules(mods) {
        mods.forEach(mod => {
            this.registerPluginModule(mod);
        });
    }
    /**
     * Register the underlying lite service manager for this app.
     *
     * @param serviceManager The Service Manager for the app.
     */
    registerServiceManager(serviceManager) {
        this._serviceManager = serviceManager;
    }
}


/***/ }),

/***/ 51276:
/*!******************************************!*\
  !*** ../../packages/server/lib/index.js ***!
  \******************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "JupyterLiteServer": () => (/* reexport safe */ _app__WEBPACK_IMPORTED_MODULE_0__.JupyterLiteServer),
/* harmony export */   "JupyterServer": () => (/* reexport safe */ _server__WEBPACK_IMPORTED_MODULE_1__.JupyterServer),
/* harmony export */   "LiteServiceManager": () => (/* reexport safe */ _service__WEBPACK_IMPORTED_MODULE_2__.LiteServiceManager)
/* harmony export */ });
/* harmony import */ var _app__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! ./app */ 88277);
/* harmony import */ var _server__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ./server */ 10255);
/* harmony import */ var _service__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ./service */ 19648);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.





/***/ }),

/***/ 96905:
/*!*******************************************!*\
  !*** ../../packages/server/lib/router.js ***!
  \*******************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "Router": () => (/* binding */ Router)
/* harmony export */ });
/**
 * A simple router.
 */
class Router {
    constructor() {
        this._routes = [];
    }
    /**
     * Add a new GET route
     *
     * @param pattern The pattern to match
     * @param callback The function to call on pattern match
     *
     */
    get(pattern, callback) {
        this._add('GET', pattern, callback);
    }
    /**
     * Add a new PUT route
     *
     * @param pattern The pattern to match
     * @param callback The function to call on pattern match
     *
     */
    put(pattern, callback) {
        this._add('PUT', pattern, callback);
    }
    /**
     * Add a new POST route
     *
     * @param pattern The pattern to match
     * @param callback The function to call on pattern match
     *
     */
    post(pattern, callback) {
        this._add('POST', pattern, callback);
    }
    /**
     * Add a new PATCH route
     *
     * @param pattern The pattern to match
     * @param callback The function to call on pattern match
     *
     */
    patch(pattern, callback) {
        this._add('PATCH', pattern, callback);
    }
    /**
     * Add a new DELETE route
     *
     * @param pattern The pattern to match
     * @param callback The function to call on pattern match
     *
     */
    delete(pattern, callback) {
        this._add('DELETE', pattern, callback);
    }
    /**
     * Route a request.
     *
     * @param req The request to route.
     */
    async route(req) {
        const url = new URL(req.url);
        const { method } = req;
        const { pathname } = url;
        for (const r of this._routes) {
            if (r.method !== method) {
                continue;
            }
            const match = pathname.match(r.pattern);
            if (!match) {
                continue;
            }
            const matches = match.slice(1);
            let body;
            if (r.method === 'PATCH' || r.method === 'PUT' || r.method === 'POST') {
                try {
                    body = JSON.parse(await req.text());
                }
                catch {
                    body = undefined;
                }
            }
            return r.callback.call(null, {
                pathname,
                body,
                query: Object.fromEntries(url.searchParams)
            }, ...matches);
        }
        throw new Error('Cannot route ' + req.method + ' ' + req.url);
    }
    /**
     * Add a new route.
     *
     * @param method The method
     * @param pattern The pattern
     * @param callback The callback
     */
    _add(method, pattern, callback) {
        if (typeof pattern === 'string') {
            pattern = new RegExp(pattern);
        }
        this._routes.push({
            method,
            pattern,
            callback
        });
    }
}


/***/ }),

/***/ 10255:
/*!*******************************************!*\
  !*** ../../packages/server/lib/server.js ***!
  \*******************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "JupyterServer": () => (/* binding */ JupyterServer)
/* harmony export */ });
/* harmony import */ var _router__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! ./router */ 96905);

/**
 * A (very, very) simplified Jupyter Server running in the browser.
 */
class JupyterServer {
    /**
     * Construct a new JupyterServer.
     */
    constructor(options) {
        this._router = new _router__WEBPACK_IMPORTED_MODULE_0__.Router();
        const { contents, kernels, kernelspecs, sessions, settings } = options;
        this._contents = contents;
        this._kernels = kernels;
        this._kernelspecs = kernelspecs;
        this._sessions = sessions;
        this._settings = settings;
        this._addRoutes();
    }
    /**
     * Handle an incoming request from the client.
     *
     * @param req The incoming request
     * @param init The optional init request
     */
    async fetch(req, init) {
        if (!(req instanceof Request)) {
            throw Error('Request info is not a Request');
        }
        return this._router.route(req);
    }
    /**
     * Add the routes.
     */
    _addRoutes() {
        const app = this._router;
        // GET /api/contents/{path}/checkpoints - Get a list of checkpoints for a file
        app.get('/api/contents/(.+)/checkpoints', async (req, filename) => {
            const res = await this._contents.listCheckpoints(filename);
            return new Response(JSON.stringify(res));
        });
        // POST /api/contents/{path}/checkpoints/{checkpoint_id} - Restore a file to a particular checkpointed state
        app.post('/api/contents/(.+)/checkpoints/(.*)', async (req, filename, checkpoint) => {
            const res = await this._contents.restoreCheckpoint(filename, checkpoint);
            return new Response(JSON.stringify(res), { status: 204 });
        });
        // POST /api/contents/{path}/checkpoints - Create a new checkpoint for a file
        app.post('/api/contents/(.+)/checkpoints', async (req, filename) => {
            const res = await this._contents.createCheckpoint(filename);
            return new Response(JSON.stringify(res), { status: 201 });
        });
        // DELETE /api/contents/{path}/checkpoints/{checkpoint_id} - Delete a checkpoint
        app.delete('/api/contents/(.+)/checkpoints/(.*)', async (req, filename, checkpoint) => {
            const res = await this._contents.deleteCheckpoint(filename, checkpoint);
            return new Response(JSON.stringify(res), { status: 204 });
        });
        // GET /api/contents/{path} - Get contents of file or directory
        app.get('/api/contents(.*)', async (req, filename) => {
            var _a;
            const options = {
                content: ((_a = req.query) === null || _a === void 0 ? void 0 : _a.content) === '1'
            };
            const nb = await this._contents.get(filename, options);
            return new Response(JSON.stringify(nb));
        });
        // POST /api/contents/{path} - Create a new file in the specified path
        app.post('/api/contents(.*)', async (req, path) => {
            const options = req.body;
            const copyFrom = options === null || options === void 0 ? void 0 : options.copy_from;
            let file;
            if (copyFrom) {
                file = await this._contents.copy(copyFrom, path);
            }
            else {
                file = await this._contents.newUntitled(options);
            }
            return new Response(JSON.stringify(file), { status: 201 });
        });
        // PATCH /api/contents/{path} - Rename a file or directory without re-uploading content
        app.patch('/api/contents(.*)', async (req, filename) => {
            var _a, _b;
            const newPath = (_b = (_a = req.body) === null || _a === void 0 ? void 0 : _a.path) !== null && _b !== void 0 ? _b : '';
            filename = filename[0] === '/' ? filename.slice(1) : filename;
            const nb = await this._contents.rename(filename, newPath);
            return new Response(JSON.stringify(nb));
        });
        // PUT /api/contents/{path} - Save or upload a file
        app.put('/api/contents/(.+)', async (req, filename) => {
            const body = req.body;
            const nb = await this._contents.save(filename, body);
            return new Response(JSON.stringify(nb));
        });
        // DELETE /api/contents/{path} - Delete a file in the given path
        app.delete('/api/contents/(.+)', async (req, filename) => {
            await this._contents.delete(filename);
            return new Response(null, { status: 204 });
        });
        // POST /api/kernels/{kernel_id} - Restart a kernel
        app.post('/api/kernels/(.*)/restart', async (req, kernelId) => {
            const res = await this._kernels.restart(kernelId);
            return new Response(JSON.stringify(res));
        });
        // DELETE /api/kernels/{kernel_id} - Kill a kernel and delete the kernel id
        app.delete('/api/kernels/(.*)', async (req, kernelId) => {
            const res = await this._kernels.shutdown(kernelId);
            return new Response(JSON.stringify(res), { status: 204 });
        });
        // KernelSpecs
        app.get('/api/kernelspecs', async (req) => {
            const res = this._kernelspecs.specs;
            return new Response(JSON.stringify(res));
        });
        // NbConvert
        app.get('/api/nbconvert', async (req) => {
            return new Response(JSON.stringify({}));
        });
        // GET /api/sessions/{session} - Get session
        app.get('/api/sessions/(.+)', async (req, id) => {
            const session = await this._sessions.get(id);
            return new Response(JSON.stringify(session), { status: 200 });
        });
        // GET /api/sessions - List available sessions
        app.get('/api/sessions', async (req) => {
            const sessions = await this._sessions.list();
            return new Response(JSON.stringify(sessions), { status: 200 });
        });
        // PATCH /api/sessions/{session} - This can be used to rename a session
        app.patch('/api/sessions(.*)', async (req, id) => {
            const options = req.body;
            const session = await this._sessions.patch(options);
            return new Response(JSON.stringify(session), { status: 200 });
        });
        // DELETE /api/sessions/{session} - Delete a session
        app.delete('/api/sessions/(.+)', async (req, id) => {
            await this._sessions.shutdown(id);
            return new Response(null, { status: 204 });
        });
        // POST /api/sessions - Create a new session or return an existing session if a session of the same name already exists
        app.post('/api/sessions', async (req) => {
            const options = req.body;
            const session = await this._sessions.startNew(options);
            return new Response(JSON.stringify(session), { status: 201 });
        });
        // Settings
        // TODO: improve the regex
        // const pluginPattern = new RegExp(/(?:@([^/]+?)[/])?([^/]+?):(\w+)/);
        const pluginPattern = '/api/settings/((?:@([^/]+?)[/])?([^/]+?):([^:]+))$';
        app.get(pluginPattern, async (req, pluginId) => {
            const settings = await this._settings.get(pluginId);
            return new Response(JSON.stringify(settings));
        });
        app.put(pluginPattern, async (req, pluginId) => {
            const body = req.body;
            const { raw } = body;
            await this._settings.save(pluginId, raw);
            return new Response(null, { status: 204 });
        });
        app.get('/api/settings', async (req) => {
            const plugins = await this._settings.getAll();
            return new Response(JSON.stringify(plugins));
        });
    }
}


/***/ }),

/***/ 19648:
/*!********************************************!*\
  !*** ../../packages/server/lib/service.js ***!
  \********************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "LiteServiceManager": () => (/* binding */ LiteServiceManager)
/* harmony export */ });
/* harmony import */ var _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/services */ 56624);
/* harmony import */ var _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var mock_socket__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! mock-socket */ 79988);
/* harmony import */ var mock_socket__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(mock_socket__WEBPACK_IMPORTED_MODULE_1__);


/**
 * A custom ServiceManager to run in the browser
 */
class LiteServiceManager extends _jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.ServiceManager {
    /**
     * Construct a new service provider.
     *
     * @param options The instantiation options for the service manager.
     */
    constructor(options) {
        var _a;
        const server = options.server;
        super({
            ...options,
            serverSettings: {
                ..._jupyterlab_services__WEBPACK_IMPORTED_MODULE_0__.ServerConnection.makeSettings(),
                WebSocket: mock_socket__WEBPACK_IMPORTED_MODULE_1__.WebSocket,
                fetch: (_a = server.fetch.bind(server)) !== null && _a !== void 0 ? _a : undefined
            }
        });
        this._server = server;
    }
    /**
     * Get the underlying Jupyter Server
     */
    get server() {
        return this._server;
    }
}


/***/ })

}]);
//# sourceMappingURL=packages_server_lib_index_js.b6251aebf368b78b0809.js.map