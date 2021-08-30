(self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] = self["webpackChunk_JUPYTERLAB_CORE_OUTPUT"] || []).push([["packages_server-extension_lib_index_js"],{

/***/ 9243:
/*!***********************************************!*\
  !*** ../../packages/contents/lib/contents.js ***!
  \***********************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "Contents": () => (/* binding */ Contents)
/* harmony export */ });
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/coreutils */ 78548);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlab_services__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlab/services */ 44650);
/* harmony import */ var _jupyterlab_services__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_services__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var _lumino_signaling__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @lumino/signaling */ 75138);
/* harmony import */ var _lumino_signaling__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(_lumino_signaling__WEBPACK_IMPORTED_MODULE_2__);
/* harmony import */ var localforage__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! localforage */ 75486);
/* harmony import */ var localforage__WEBPACK_IMPORTED_MODULE_3___default = /*#__PURE__*/__webpack_require__.n(localforage__WEBPACK_IMPORTED_MODULE_3__);





/**
 * The name of the local storage.
 */
const STORAGE_NAME = 'JupyterLite Storage';
/**
 * The number of checkpoints to save.
 */
const N_CHECKPOINTS = 5;
/**
 * A class to handle requests to /api/contents
 */
class Contents {
    constructor() {
        this._serverContents = new Map();
        this._isDisposed = false;
        this._fileChanged = new _lumino_signaling__WEBPACK_IMPORTED_MODULE_2__.Signal(this);
        this._storage = localforage__WEBPACK_IMPORTED_MODULE_3___default().createInstance({
            name: STORAGE_NAME,
            description: 'Offline Storage for Notebooks and Files',
            storeName: 'files',
            version: 1
        });
        this._counters = localforage__WEBPACK_IMPORTED_MODULE_3___default().createInstance({
            name: STORAGE_NAME,
            description: 'Store the current file suffix counters',
            storeName: 'counters',
            version: 1
        });
        this._checkpoints = localforage__WEBPACK_IMPORTED_MODULE_3___default().createInstance({
            name: STORAGE_NAME,
            description: 'Offline Storage for Checkpoints',
            storeName: 'checkpoints',
            version: 1
        });
    }
    /**
     * A signal emitted when the file has changed.
     */
    get fileChanged() {
        return this._fileChanged;
    }
    /**
     * Test whether the manager has been disposed.
     */
    get isDisposed() {
        return this._isDisposed;
    }
    /**
     * Return the server settings.
     */
    get serverSettings() {
        // TODO: placeholder
        return _jupyterlab_services__WEBPACK_IMPORTED_MODULE_1__.ServerConnection.makeSettings();
    }
    /**
     * Dispose of the resources held by the manager.
     */
    dispose() {
        throw new Error('Method not implemented.');
    }
    /**
     * Create a new untitled file or directory in the specified directory path.
     *
     * @param options: The options used to create the file.
     *
     * @returns A promise which resolves with the created file content when the file is created.
     */
    async newUntitled(options) {
        var _a, _b, _c;
        const path = (_a = options === null || options === void 0 ? void 0 : options.path) !== null && _a !== void 0 ? _a : '';
        const type = (_b = options === null || options === void 0 ? void 0 : options.type) !== null && _b !== void 0 ? _b : 'notebook';
        const created = new Date().toISOString();
        const prefix = path ? `${path}/` : '';
        let file;
        let name = '';
        switch (type) {
            case 'directory': {
                const counter = await this._incrementCounter('directory');
                name += `Untitled Folder${counter || ''}`;
                file = {
                    name,
                    path: `${prefix}${name}`,
                    last_modified: created,
                    created,
                    format: 'text',
                    mimetype: '',
                    content: null,
                    size: undefined,
                    writable: true,
                    type: 'directory'
                };
                break;
            }
            case 'file': {
                const ext = (_c = options === null || options === void 0 ? void 0 : options.ext) !== null && _c !== void 0 ? _c : '.txt';
                const counter = await this._incrementCounter('file');
                name += `untitled${counter || ''}${ext}`;
                file = {
                    name,
                    path: `${prefix}${name}`,
                    last_modified: created,
                    created,
                    format: 'text',
                    // TODO: handle mimetypes
                    mimetype: 'text/plain',
                    content: '',
                    size: 0,
                    writable: true,
                    type: 'file'
                };
                break;
            }
            default: {
                const counter = await this._incrementCounter('notebook');
                name += `Untitled${counter || ''}.ipynb`;
                file = {
                    name,
                    path: `${prefix}${name}`,
                    last_modified: created,
                    created,
                    format: 'json',
                    mimetype: 'application/json',
                    content: Private.EMPTY_NB,
                    size: JSON.stringify(Private.EMPTY_NB).length,
                    writable: true,
                    type: 'notebook'
                };
                break;
            }
        }
        const key = `${prefix}${name}`;
        await this._storage.setItem(key, file);
        return file;
    }
    /**
     * Copy a file into a given directory.
     *
     * @param path - The original file path.
     * @param toDir - The destination directory path.
     *
     * @returns A promise which resolves with the new contents model when the
     *  file is copied.
     *
     * #### Notes
     * The server will select the name of the copied file.
     */
    async copy(path, toDir) {
        let name = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PathExt.basename(path);
        toDir = toDir === '' ? '' : `${toDir.slice(1)}/`;
        // TODO: better handle naming collisions with existing files
        while (await this.get(`${toDir}${name}`, { content: true })) {
            const ext = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PathExt.extname(name);
            const base = name.replace(ext, '');
            name = `${base} (copy)${ext}`;
        }
        const toPath = `${toDir}${name}`;
        let item = await this.get(path, { content: true });
        item = {
            ...item,
            name,
            path: toPath
        };
        await this._storage.setItem(toPath, item);
        return item;
    }
    /**
     * Get a file or directory.
     *
     * @param path: The path to the file.
     * @param options: The options used to fetch the file.
     *
     * @returns A promise which resolves with the file content.
     */
    async get(path, options) {
        // remove leading slash
        path = decodeURIComponent(path.replace(/^\//, ''));
        if (path === '') {
            return await this.getFolder(path);
        }
        const item = await this._storage.getItem(path);
        const serverItem = await this.getServerContents(path, options);
        const model = (item || serverItem);
        if (!model) {
            throw Error(`Could not find file with path ${path}`);
        }
        if (!(options === null || options === void 0 ? void 0 : options.content)) {
            return {
                ...model,
                content: null,
                size: undefined
            };
        }
        // for directories, find all files with the path as the prefix
        if (model.type === 'directory') {
            const contentMap = new Map();
            await this._storage.iterate((item, key) => {
                const file = item;
                // use an additional slash to not include the directory itself
                if (key === `${path}/${file.name}`) {
                    contentMap.set(file.name, file);
                }
            });
            const serverContents = serverItem
                ? serverItem.content
                : Array.from((await this.getServerDirectory(path)).values());
            for (const file of serverContents) {
                if (!contentMap.has(file.name)) {
                    contentMap.set(file.name, file);
                }
            }
            const content = [...contentMap.values()];
            return {
                name: _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PathExt.basename(path),
                path,
                last_modified: model.last_modified,
                created: model.created,
                format: 'json',
                mimetype: 'application/json',
                content,
                size: undefined,
                writable: true,
                type: 'directory'
            };
        }
        return model;
    }
    /**
     * retrieve the contents for this path from the union of local storage and
     * `api/contents/{path}/all.json`.
     *
     * @param path - The contents path to retrieve
     *
     * @returns A promise which resolves with a Map of contents, keyed by local file name
     */
    async getFolder(path) {
        const content = new Map();
        await this._storage.iterate((item, key) => {
            if (key.includes('/')) {
                return;
            }
            const file = item;
            content.set(file.path, file);
        });
        // layer in contents that don't have local overwrites
        for (const file of (await this.getServerDirectory(path)).values()) {
            if (!content.has(file.path)) {
                content.set(file.path, file);
            }
        }
        return {
            name: '',
            path,
            last_modified: new Date(0).toISOString(),
            created: new Date(0).toISOString(),
            format: 'json',
            mimetype: 'application/json',
            content: Array.from(content.values()),
            size: undefined,
            writable: true,
            type: 'directory'
        };
    }
    /**
     * retrieve the contents for this path from `__index__.json` in the appropriate
     * folder.
     *
     * @param newLocalPath - The new file path.
     *
     * @returns A promise which resolves with a Map of contents, keyed by local file name
     */
    async getServerDirectory(path) {
        const content = this._serverContents.get(path) || new Map();
        if (!this._serverContents.has(path)) {
            const apiURL = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PageConfig.getBaseUrl(), 'api/contents', path, 'all.json');
            try {
                const response = await fetch(apiURL);
                const json = JSON.parse(await response.text());
                for (const file of json['content']) {
                    content.set(file.name, file);
                }
            }
            catch (err) {
                console.warn(`don't worry, about ${err}... nothing's broken. if there had been a
          file at ${apiURL}, you might see some more files.`);
            }
            this._serverContents.set(path, content);
        }
        return content;
    }
    /**
     * Attempt to recover the model from `{:path}/__all__.json` file, fall back to
     * deriving the model (including content) off the file in `/files/`. Otherwise
     * return `null`.
     */
    async getServerContents(path, options) {
        const name = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PathExt.basename(path);
        const parentContents = await this.getServerDirectory(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(path, '..'));
        let model = parentContents.get(name) || {
            name,
            path,
            last_modified: new Date(0).toISOString(),
            created: new Date(0).toISOString(),
            format: 'text',
            mimetype: 'text/plain',
            type: 'file',
            writable: true,
            content: null
        };
        if (options === null || options === void 0 ? void 0 : options.content) {
            if (model.type === 'directory') {
                const serverContents = await this.getServerDirectory(path);
                model = { ...model, content: Array.from(serverContents.values()) };
            }
            else {
                const fileUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PageConfig.getBaseUrl(), 'files', path);
                const response = await fetch(fileUrl);
                if (!response.ok) {
                    return null;
                }
                const mimetype = model.mimetype || response.headers.get('Content-Type');
                if (model.type === 'notebook' ||
                    (mimetype === null || mimetype === void 0 ? void 0 : mimetype.indexOf('json')) !== -1 ||
                    path.match(/\.(ipynb|[^/]*json[^/]*)$/)) {
                    model = {
                        ...model,
                        content: await response.json(),
                        format: 'json',
                        mimetype: model.mimetype || 'application/json'
                    };
                    // TODO: this is not great, need a better oracle
                }
                else if (mimetype === 'image/svg+xml' || mimetype.indexOf('text') !== -1) {
                    model = {
                        ...model,
                        content: await response.text(),
                        format: 'text',
                        mimetype: mimetype || 'text/plain'
                    };
                }
                else {
                    model = {
                        ...model,
                        content: btoa(String.fromCharCode(...new Uint8Array(await response.arrayBuffer()))),
                        format: 'base64',
                        mimetype: mimetype || 'octet/stream'
                    };
                }
            }
        }
        return model;
    }
    /**
     * Rename a file or directory.
     *
     * @param oldLocalPath - The original file path.
     * @param newLocalPath - The new file path.
     *
     * @returns A promise which resolves with the new file content model when the file is renamed.
     */
    async rename(oldLocalPath, newLocalPath) {
        const path = decodeURIComponent(oldLocalPath);
        const file = await this.get(path, { content: true });
        if (!file) {
            throw Error(`Could not find file with path ${path}`);
        }
        const modified = new Date().toISOString();
        const name = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PathExt.basename(newLocalPath);
        const newFile = {
            ...file,
            name,
            path: newLocalPath,
            last_modified: modified
        };
        await this._storage.setItem(newLocalPath, newFile);
        // remove the old file
        await this._storage.removeItem(path);
        // remove the corresponding checkpoint
        await this._checkpoints.removeItem(path);
        // if a directory, recurse through all children
        if (file.type === 'directory') {
            let child;
            for (child of file.content) {
                await this.rename(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(oldLocalPath, child.name), _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(newLocalPath, child.name));
            }
        }
        return newFile;
    }
    /**
     * Save a file.
     *
     * @param path - The desired file path.
     * @param options - Optional overrides to the model.
     *
     * @returns A promise which resolves with the file content model when the file is saved.
     */
    async save(path, options = {}) {
        var _a;
        let item = await this.get(path);
        if (!item) {
            item = await this.newUntitled({ path });
        }
        // override with the new values
        const modified = new Date().toISOString();
        item = {
            ...item,
            ...options,
            last_modified: modified
        };
        // process the file if coming from an upload
        const ext = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PathExt.extname((_a = options.name) !== null && _a !== void 0 ? _a : '');
        if (options.content && options.format === 'base64') {
            // TODO: keep base64 if not a text file (image)
            const content = atob(options.content);
            const nb = ext === '.ipynb';
            item = {
                ...item,
                content: nb ? JSON.parse(content) : content,
                format: nb ? 'json' : 'text',
                type: nb ? 'notebook' : 'file'
            };
        }
        await this._storage.setItem(path, item);
        return item;
    }
    /**
     * Delete a file.
     *
     * @param path - The path to the file.
     */
    async delete(path) {
        path = decodeURIComponent(path);
        const toDelete = [];
        // handle deleting directories recursively
        await this._storage.iterate((item, key) => {
            if (key === path || key.startsWith(`${path}/`)) {
                toDelete.push(key);
            }
        });
        await Promise.all(toDelete.map(async (p) => {
            return Promise.all([
                this._storage.removeItem(p),
                this._checkpoints.removeItem(p)
            ]);
        }));
    }
    /**
     * Create a checkpoint for a file.
     *
     * @param path - The path of the file.
     *
     * @returns A promise which resolves with the new checkpoint model when the
     *   checkpoint is created.
     */
    async createCheckpoint(path) {
        var _a;
        const item = await this.get(path, { content: true });
        const copies = ((_a = (await this._checkpoints.getItem(path))) !== null && _a !== void 0 ? _a : []).filter(item => !!item);
        copies.push(item);
        // keep only a certain amount of checkpoints per file
        if (copies.length > N_CHECKPOINTS) {
            copies.splice(0, copies.length - N_CHECKPOINTS);
        }
        await this._checkpoints.setItem(path, copies);
        const id = `${copies.length - 1}`;
        return {
            id,
            last_modified: item.last_modified
        };
    }
    /**
     * List available checkpoints for a file.
     *
     * @param path - The path of the file.
     *
     * @returns A promise which resolves with a list of checkpoint models for
     *    the file.
     */
    async listCheckpoints(path) {
        const copies = ((await this._checkpoints.getItem(path)) ||
            []);
        return copies
            .filter(item => !!item)
            .map((file, id) => {
            return {
                id: id.toString(),
                last_modified: file.last_modified
            };
        });
    }
    /**
     * Restore a file to a known checkpoint state.
     *
     * @param path - The path of the file.
     * @param checkpointID - The id of the checkpoint to restore.
     *
     * @returns A promise which resolves when the checkpoint is restored.
     */
    async restoreCheckpoint(path, checkpointID) {
        const copies = ((await this._checkpoints.getItem(path)) ||
            []);
        const id = parseInt(checkpointID);
        const item = copies[id];
        await this._storage.setItem(path, item);
    }
    /**
     * Delete a checkpoint for a file.
     *
     * @param path - The path of the file.
     * @param checkpointID - The id of the checkpoint to delete.
     *
     * @returns A promise which resolves when the checkpoint is deleted.
     */
    async deleteCheckpoint(path, checkpointID) {
        const copies = ((await this._checkpoints.getItem(path)) ||
            []);
        const id = parseInt(checkpointID);
        copies.splice(id, 1);
        await this._checkpoints.setItem(path, copies);
    }
    /**
     * Add an `IDrive` to the manager.
     */
    addDrive(drive) {
        throw new Error('Method not implemented.');
    }
    /**
     * Given a path of the form `drive:local/portion/of/it.txt`
     * get the local part of it.
     *
     * @param path: the path.
     *
     * @returns The local part of the path.
     */
    localPath(path) {
        throw new Error('Method not implemented.');
    }
    /**
     * Normalize a global path. Reduces '..' and '.' parts, and removes
     * leading slashes from the local part of the path, while retaining
     * the drive name if it exists.
     *
     * @param path: the path.
     *
     * @returns The normalized path.
     */
    normalize(path) {
        throw new Error('Method not implemented.');
    }
    /**
     * Resolve a global path, starting from the root path. Behaves like
     * posix-path.resolve, with 3 differences:
     *  - will never prepend cwd
     *  - if root has a drive name, the result is prefixed with "<drive>:"
     *  - before adding drive name, leading slashes are removed
     *
     * @param path: the path.
     *
     * @returns The normalized path.
     */
    resolvePath(root, path) {
        throw new Error('Method not implemented.');
    }
    /**
     * Given a path of the form `drive:local/portion/of/it.txt`
     * get the name of the drive. If the path is missing
     * a drive portion, returns an empty string.
     *
     * @param path: the path.
     *
     * @returns The drive name for the path, or the empty string.
     */
    driveName(path) {
        throw new Error('Method not implemented.');
    }
    /**
     * Given a path, get a ModelDB.IFactory from the
     * relevant backend. Returns `null` if the backend
     * does not provide one.
     */
    getModelDBFactory(path) {
        throw new Error('Method not implemented.');
    }
    /**
     * Get an encoded download url given a file path.
     *
     * @param path - An absolute POSIX file path on the server.
     *
     * #### Notes
     * It is expected that the path contains no relative paths.
     *
     * The returned URL may include a query parameter.
     */
    getDownloadUrl(path) {
        throw new Error('Method not implemented.');
    }
    /**
     * Increment the counter for a given file type.
     * Used to avoid collisions when creating new untitled files.
     *
     * @param type The file type to increment the counter for.
     */
    async _incrementCounter(type) {
        var _a;
        const current = (_a = (await this._counters.getItem(type))) !== null && _a !== void 0 ? _a : -1;
        const counter = current + 1;
        await this._counters.setItem(type, counter);
        return counter;
    }
}
/**
 * A namespace for private data.
 */
var Private;
(function (Private) {
    /**
     * The content for an empty notebook.
     */
    Private.EMPTY_NB = {
        metadata: {
            orig_nbformat: 4
        },
        nbformat_minor: 4,
        nbformat: 4,
        cells: []
    };
})(Private || (Private = {}));


/***/ }),

/***/ 87830:
/*!*********************************************!*\
  !*** ../../packages/contents/lib/tokens.js ***!
  \*********************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "IContents": () => (/* binding */ IContents)
/* harmony export */ });
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/coreutils */ 9727);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__);

/**
 * The token for the settings service.
 */
const IContents = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.Token('@jupyterlite/contents:IContents');


/***/ }),

/***/ 82:
/*!********************************************!*\
  !*** ../../packages/kernel/lib/kernels.js ***!
  \********************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "Kernels": () => (/* binding */ Kernels)
/* harmony export */ });
/* harmony import */ var _jupyterlab_observables__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/observables */ 39338);
/* harmony import */ var _jupyterlab_observables__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_observables__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlab_services_lib_kernel_serialize__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlab/services/lib/kernel/serialize */ 86041);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @lumino/coreutils */ 9727);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_2__);
/* harmony import */ var mock_socket__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! mock-socket */ 79988);
/* harmony import */ var mock_socket__WEBPACK_IMPORTED_MODULE_3___default = /*#__PURE__*/__webpack_require__.n(mock_socket__WEBPACK_IMPORTED_MODULE_3__);
/* harmony import */ var async_mutex__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! async-mutex */ 4810);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! @jupyterlab/coreutils */ 78548);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_5___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_5__);






/**
 * A class to handle requests to /api/kernels
 */
class Kernels {
    /**
     * Construct a new Kernels
     *
     * @param options The instantiation options
     */
    constructor(options) {
        this._kernels = new _jupyterlab_observables__WEBPACK_IMPORTED_MODULE_0__.ObservableMap();
        this._clients = new _jupyterlab_observables__WEBPACK_IMPORTED_MODULE_0__.ObservableMap();
        this._kernelClients = new _jupyterlab_observables__WEBPACK_IMPORTED_MODULE_0__.ObservableMap();
        const { kernelspecs } = options;
        this._kernelspecs = kernelspecs;
    }
    /**
     * Start a new kernel.
     *
     * @param options The kernel start options.
     */
    async startNew(options) {
        const { id, name } = options;
        const factory = this._kernelspecs.factories.get(name);
        // bail if there is no factory associated with the requested kernel
        if (!factory) {
            return { id, name };
        }
        // create a synchronization mechanism to allow only one message
        // to be processed at a time
        const mutex = new async_mutex__WEBPACK_IMPORTED_MODULE_4__.Mutex();
        // hook a new client to a kernel
        const hook = (kernelId, clientId, socket) => {
            var _a;
            const kernel = this._kernels.get(kernelId);
            if (!kernel) {
                throw Error(`No kernel ${kernelId}`);
            }
            this._clients.set(clientId, socket);
            (_a = this._kernelClients.get(kernelId)) === null || _a === void 0 ? void 0 : _a.add(clientId);
            const processMsg = async (msg) => {
                await mutex.runExclusive(async () => {
                    await kernel.handleMessage(msg);
                });
            };
            socket.on('message', async (message) => {
                let msg;
                if (message instanceof ArrayBuffer) {
                    message = new Uint8Array(message).buffer;
                    msg = (0,_jupyterlab_services_lib_kernel_serialize__WEBPACK_IMPORTED_MODULE_1__.deserialize)(message);
                }
                else if (typeof message === 'string') {
                    msg = (0,_jupyterlab_services_lib_kernel_serialize__WEBPACK_IMPORTED_MODULE_1__.deserialize)(message);
                }
                else {
                    return;
                }
                // TODO Find a better solution for this?
                // input-reply is asynchronous, must not be processed like other messages
                if (msg.header.msg_type === 'input_reply') {
                    kernel.handleMessage(msg);
                }
                else {
                    void processMsg(msg);
                }
            });
            const removeClient = () => {
                var _a;
                this._clients.delete(clientId);
                (_a = this._kernelClients.get(kernelId)) === null || _a === void 0 ? void 0 : _a.delete(clientId);
            };
            kernel.disposed.connect(removeClient);
            // TODO: check whether this is called
            // https://github.com/thoov/mock-socket/issues/298
            // https://github.com/jupyterlab/jupyterlab/blob/6bc884a7a8ed73c615ce72ba097bdb790482b5bf/packages/services/src/kernel/default.ts#L1245
            socket.onclose = removeClient;
        };
        // ensure kernel id
        const kernelId = id !== null && id !== void 0 ? id : _lumino_coreutils__WEBPACK_IMPORTED_MODULE_2__.UUID.uuid4();
        // There is one server per kernel which handles multiple clients
        const kernelUrl = `${Kernels.WS_BASE_URL}api/kernels/${kernelId}/channels`;
        const runningKernel = this._kernels.get(kernelId);
        if (runningKernel) {
            return {
                id: runningKernel.id,
                name: runningKernel.name
            };
        }
        // start the kernel
        const sendMessage = (msg) => {
            const clientId = msg.header.session;
            const socket = this._clients.get(clientId);
            if (!socket) {
                console.warn(`Trying to send message on removed socket for kernel ${kernelId}`);
                return;
            }
            const message = (0,_jupyterlab_services_lib_kernel_serialize__WEBPACK_IMPORTED_MODULE_1__.serialize)(msg);
            // process iopub messages
            if (msg.channel === 'iopub') {
                const clients = this._kernelClients.get(kernelId);
                clients === null || clients === void 0 ? void 0 : clients.forEach(id => {
                    var _a;
                    (_a = this._clients.get(id)) === null || _a === void 0 ? void 0 : _a.send(message);
                });
                return;
            }
            socket.send(message);
        };
        const kernel = await factory({
            id: kernelId,
            sendMessage,
            name
        });
        await kernel.ready;
        this._kernels.set(kernelId, kernel);
        this._kernelClients.set(kernelId, new Set());
        // create the websocket server for the kernel
        const wsServer = new mock_socket__WEBPACK_IMPORTED_MODULE_3__.Server(kernelUrl);
        wsServer.on('connection', (socket) => {
            var _a;
            const url = new URL(socket.url);
            const clientId = (_a = url.searchParams.get('session_id')) !== null && _a !== void 0 ? _a : '';
            hook(kernelId, clientId, socket);
        });
        // clean up closed connection
        wsServer.on('close', () => {
            this._clients.keys().forEach(clientId => {
                var _a;
                const socket = this._clients.get(clientId);
                if ((socket === null || socket === void 0 ? void 0 : socket.readyState) === mock_socket__WEBPACK_IMPORTED_MODULE_3__.WebSocket.CLOSED) {
                    this._clients.delete(clientId);
                    (_a = this._kernelClients.get(kernelId)) === null || _a === void 0 ? void 0 : _a.delete(clientId);
                }
            });
        });
        // cleanup on kernel shutdown
        kernel.disposed.connect(() => {
            wsServer.close();
            this._kernels.delete(kernelId);
            this._kernelClients.delete(kernelId);
        });
        return {
            id: kernel.id,
            name: kernel.name
        };
    }
    /**
     * Restart a kernel.
     *
     * @param kernelId The kernel id.
     */
    async restart(kernelId) {
        const kernel = this._kernels.get(kernelId);
        if (!kernel) {
            throw Error(`Kernel ${kernelId} does not exist`);
        }
        const { id, name } = kernel;
        kernel.dispose();
        return this.startNew({ id, name });
    }
    /**
     * Shut down a kernel.
     *
     * @param id The kernel id.
     */
    async shutdown(id) {
        var _a;
        (_a = this._kernels.delete(id)) === null || _a === void 0 ? void 0 : _a.dispose();
    }
}
/**
 * A namespace for Kernels statics.
 */
(function (Kernels) {
    /**
     * The base url for the Kernels manager
     */
    Kernels.WS_BASE_URL = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_5__.PageConfig.getBaseUrl().replace(/^http/, 'ws');
})(Kernels || (Kernels = {}));


/***/ }),

/***/ 94011:
/*!************************************************!*\
  !*** ../../packages/kernel/lib/kernelspecs.js ***!
  \************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "KernelSpecs": () => (/* binding */ KernelSpecs)
/* harmony export */ });
/**
 * A class to handle requests to /api/kernelspecs
 */
class KernelSpecs {
    /**
     * Construct a new KernelSpecs.
     *
     * @param options The instantiation options.
     */
    constructor(options) {
        this._specs = new Map();
        this._factories = new Map();
        // no-op
    }
    /**
     * Get the kernel specs.
     */
    get specs() {
        if (this._specs.size === 0) {
            return null;
        }
        return {
            default: 'python',
            kernelspecs: Object.fromEntries(this._specs)
        };
    }
    /**
     * Get the kernel factories for the current kernels.
     */
    get factories() {
        return this._factories;
    }
    /**
     * Register a new kernel.
     *
     * @param options The options to register a new kernel.
     */
    register(options) {
        const { spec, create } = options;
        this._specs.set(spec.name, spec);
        this._factories.set(spec.name, create);
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
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/coreutils */ 9727);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__);

/**
 * The token for the kernels service.
 */
const IKernels = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.Token('@jupyterlite/kernel:IKernels');
/**
 * The token for the kernel spec service.
 */
const IKernelSpecs = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.Token('@jupyterlite/kernelspec:IKernelSpecs');


/***/ }),

/***/ 29786:
/*!****************************************************!*\
  !*** ../../packages/server-extension/lib/index.js ***!
  \****************************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "default": () => (__WEBPACK_DEFAULT_EXPORT__)
/* harmony export */ });
/* harmony import */ var _jupyterlite_contents__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @jupyterlite/contents */ 87830);
/* harmony import */ var _jupyterlite_contents__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @jupyterlite/contents */ 9243);
/* harmony import */ var _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! @jupyterlite/kernel */ 64831);
/* harmony import */ var _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! @jupyterlite/kernel */ 82);
/* harmony import */ var _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! @jupyterlite/kernel */ 94011);
/* harmony import */ var _jupyterlite_server__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlite/server */ 52075);
/* harmony import */ var _jupyterlite_server__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlite_server__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _jupyterlite_session__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(/*! @jupyterlite/session */ 77274);
/* harmony import */ var _jupyterlite_session__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(/*! @jupyterlite/session */ 45140);
/* harmony import */ var _jupyterlite_settings__WEBPACK_IMPORTED_MODULE_8__ = __webpack_require__(/*! @jupyterlite/settings */ 53439);
/* harmony import */ var _jupyterlite_settings__WEBPACK_IMPORTED_MODULE_9__ = __webpack_require__(/*! @jupyterlite/settings */ 61051);
// Copyright (c) Jupyter Development Team.
// Distributed under the terms of the Modified BSD License.





/**
 * The contents service plugin.
 */
const contents = {
    id: '@jupyterlite/server-extension:contents',
    autoStart: true,
    provides: _jupyterlite_contents__WEBPACK_IMPORTED_MODULE_1__.IContents,
    activate: (app) => {
        return new _jupyterlite_contents__WEBPACK_IMPORTED_MODULE_2__.Contents();
    }
};
/**
 * The kernels service plugin.
 */
const kernels = {
    id: '@jupyterlite/server-extension:kernels',
    autoStart: true,
    provides: _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_3__.IKernels,
    requires: [_jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_3__.IKernelSpecs],
    activate: (app, kernelspecs) => {
        return new _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_4__.Kernels({ kernelspecs });
    }
};
/**
 * The kernel spec service plugin.
 */
const kernelSpec = {
    id: '@jupyterlite/server-extension:kernelspec',
    autoStart: true,
    provides: _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_3__.IKernelSpecs,
    activate: (app) => {
        return new _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_5__.KernelSpecs({});
    }
};
/**
 * The sessions service plugin.
 */
const sessions = {
    id: '@jupyterlite/server-extension:sessions',
    autoStart: true,
    provides: _jupyterlite_session__WEBPACK_IMPORTED_MODULE_6__.ISessions,
    requires: [_jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_3__.IKernels],
    activate: (app, kernels) => {
        return new _jupyterlite_session__WEBPACK_IMPORTED_MODULE_7__.Sessions({ kernels });
    }
};
/**
 * The server plugin.
 */
const server = {
    id: '@jupyterlite/server-extension:server',
    autoStart: true,
    requires: [_jupyterlite_contents__WEBPACK_IMPORTED_MODULE_1__.IContents, _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_3__.IKernels, _jupyterlite_kernel__WEBPACK_IMPORTED_MODULE_3__.IKernelSpecs, _jupyterlite_session__WEBPACK_IMPORTED_MODULE_6__.ISessions, _jupyterlite_settings__WEBPACK_IMPORTED_MODULE_8__.ISettings],
    activate: (app, contents, kernels, kernelspecs, sessions, settings) => {
        const jupyterServer = new _jupyterlite_server__WEBPACK_IMPORTED_MODULE_0__.JupyterServer({
            contents,
            kernels,
            kernelspecs,
            sessions,
            settings
        });
        const serviceManager = new _jupyterlite_server__WEBPACK_IMPORTED_MODULE_0__.LiteServiceManager({ server: jupyterServer });
        app.registerServiceManager(serviceManager);
    }
};
/**
 * The settings service plugin.
 */
const settings = {
    id: '@jupyterlite/server-extension:settings',
    autoStart: true,
    provides: _jupyterlite_settings__WEBPACK_IMPORTED_MODULE_8__.ISettings,
    activate: (app) => {
        return new _jupyterlite_settings__WEBPACK_IMPORTED_MODULE_9__.Settings();
    }
};
const plugins = [
    contents,
    kernels,
    kernelSpec,
    server,
    sessions,
    settings
];
/* harmony default export */ const __WEBPACK_DEFAULT_EXPORT__ = (plugins);


/***/ }),

/***/ 45140:
/*!**********************************************!*\
  !*** ../../packages/session/lib/sessions.js ***!
  \**********************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "Sessions": () => (/* binding */ Sessions)
/* harmony export */ });
/* harmony import */ var _lumino_algorithm__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/algorithm */ 62867);
/* harmony import */ var _lumino_algorithm__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_algorithm__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @lumino/coreutils */ 9727);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_1__);


/**
 * A class to handle requests to /api/sessions
 */
class Sessions {
    /**
     * Construct a new Sessions.
     *
     * @param options The instantiation options for a Sessions.
     */
    constructor(options) {
        // TODO: offload to a database
        this._sessions = [];
        this._kernels = options.kernels;
    }
    /**
     * Get a session by id.
     *
     * @param id The id of the session.
     */
    async get(id) {
        const session = this._sessions.find(s => s.id === id);
        if (!session) {
            throw Error(`Session ${id} not found`);
        }
        return session;
    }
    /**
     * List the running sessions
     */
    async list() {
        return this._sessions;
    }
    /**
     * Path an existing session.
     * This can be used to rename a session.
     * TODO: read path and name
     *
     * @param options The options to patch the session.
     */
    async patch(options) {
        const { id, path, name } = options;
        const index = this._sessions.findIndex(s => s.id === id);
        const session = this._sessions[index];
        if (!session) {
            throw Error(`Session ${id} not found`);
        }
        const patched = {
            ...session,
            path: path !== null && path !== void 0 ? path : session.path,
            name: name !== null && name !== void 0 ? name : session.name
        };
        this._sessions[index] = patched;
        return patched;
    }
    /**
     * Start a new session
     * TODO: read path and name
     *
     * @param options The options to start a new session.
     */
    async startNew(options) {
        var _a, _b, _c;
        const { path, name } = options;
        const running = this._sessions.find(s => s.name === name);
        if (running) {
            return running;
        }
        const kernelName = (_b = (_a = options.kernel) === null || _a === void 0 ? void 0 : _a.name) !== null && _b !== void 0 ? _b : '';
        const id = (_c = options.id) !== null && _c !== void 0 ? _c : _lumino_coreutils__WEBPACK_IMPORTED_MODULE_1__.UUID.uuid4();
        const kernel = await this._kernels.startNew({ id, name: kernelName });
        const session = {
            id,
            path,
            name: name !== null && name !== void 0 ? name : path,
            type: 'notebook',
            kernel: {
                id: kernel.id,
                name: kernel.name
            }
        };
        this._sessions.push(session);
        return session;
    }
    /**
     * Shut down a session.
     *
     * @param id The id of the session to shut down.
     */
    async shutdown(id) {
        var _a;
        const session = this._sessions.find(s => s.id === id);
        if (!session) {
            throw Error(`Session ${id} not found`);
        }
        const kernelId = (_a = session.kernel) === null || _a === void 0 ? void 0 : _a.id;
        if (kernelId) {
            await this._kernels.shutdown(kernelId);
        }
        _lumino_algorithm__WEBPACK_IMPORTED_MODULE_0__.ArrayExt.removeFirstOf(this._sessions, session);
    }
}


/***/ }),

/***/ 77274:
/*!********************************************!*\
  !*** ../../packages/session/lib/tokens.js ***!
  \********************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "ISessions": () => (/* binding */ ISessions)
/* harmony export */ });
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/coreutils */ 9727);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__);

/**
 * The token for the sessions service.
 */
const ISessions = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.Token('@jupyterlite/session:ISessions');


/***/ }),

/***/ 61051:
/*!***********************************************!*\
  !*** ../../packages/settings/lib/settings.js ***!
  \***********************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "Settings": () => (/* binding */ Settings)
/* harmony export */ });
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @jupyterlab/coreutils */ 78548);
/* harmony import */ var _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var json5__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! json5 */ 96111);
/* harmony import */ var json5__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(json5__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var localforage__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! localforage */ 75486);
/* harmony import */ var localforage__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(localforage__WEBPACK_IMPORTED_MODULE_2__);



/**
 * The name of the local storage.
 */
const STORAGE_NAME = 'JupyterLite Storage';
/**
 * A class to handle requests to /api/settings
 */
class Settings {
    constructor() {
        this._storage = localforage__WEBPACK_IMPORTED_MODULE_2___default().createInstance({
            name: STORAGE_NAME,
            description: 'Offline Storage for Settings',
            storeName: 'settings',
            version: 1
        });
    }
    /**
     * Get settings by plugin id
     *
     * @param pluginId the id of the plugin
     *
     */
    async get(pluginId) {
        const all = await this.getAll();
        const settings = all.settings;
        let found = settings.find((setting) => {
            return setting.id === pluginId;
        });
        if (!found) {
            found = await this._getFederated(pluginId);
        }
        return found;
    }
    /**
     * Get all the settings
     */
    async getAll() {
        var _a;
        const settingsUrl = (_a = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PageConfig.getOption('settingsUrl')) !== null && _a !== void 0 ? _a : '/';
        const all = (await (await fetch(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(settingsUrl, 'all.json'))).json());
        const settings = await Promise.all(all.map(async (plugin) => {
            var _a;
            const { id } = plugin;
            const raw = (_a = (await this._storage.getItem(id))) !== null && _a !== void 0 ? _a : plugin.raw;
            return {
                ...Private.override(plugin),
                raw,
                settings: json5__WEBPACK_IMPORTED_MODULE_1__.parse(raw)
            };
        }));
        return { settings };
    }
    /**
     * Save settings for a given plugin id
     *
     * @param pluginId The id of the plugin
     * @param raw The raw settings
     *
     */
    async save(pluginId, raw) {
        await this._storage.setItem(pluginId, raw);
    }
    /**
     * Get the settings for a federated extension
     *
     * @param pluginId The id of a plugin
     */
    async _getFederated(pluginId) {
        var _a;
        const [packageName, schemaName] = pluginId.split(':');
        if (!Private.isFederated(packageName)) {
            return;
        }
        const labExtensionsUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PageConfig.getOption('fullLabextensionsUrl');
        const schemaUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(labExtensionsUrl, packageName, 'schemas', packageName, `${schemaName}.json`);
        const packageUrl = _jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.URLExt.join(labExtensionsUrl, packageName, 'package.json');
        const schema = await (await fetch(schemaUrl)).json();
        const packageJson = await (await fetch(packageUrl)).json();
        const raw = (_a = (await this._storage.getItem(pluginId))) !== null && _a !== void 0 ? _a : '{}';
        const settings = json5__WEBPACK_IMPORTED_MODULE_1__.parse(raw) || {};
        return Private.override({
            id: pluginId,
            raw,
            schema,
            settings,
            version: packageJson.version || '3.0.8'
        });
    }
}
/**
 * A namespace for private data
 */
var Private;
(function (Private) {
    const _overrides = JSON.parse(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PageConfig.getOption('settingsOverrides') || '{}');
    /**
     * Test whether this package is configured in `federated_extensions` in this app
     *
     * @param packageName The npm name of a package
     */
    function isFederated(packageName) {
        let federated;
        try {
            federated = JSON.parse(_jupyterlab_coreutils__WEBPACK_IMPORTED_MODULE_0__.PageConfig.getOption('federated_extensions'));
        }
        catch {
            return false;
        }
        for (const { name } of federated) {
            if (name === packageName) {
                return true;
            }
        }
        return false;
    }
    Private.isFederated = isFederated;
    /**
     * Override the defaults of the schema with ones from PageConfig
     *
     * @see https://github.com/jupyterlab/jupyterlab_server/blob/v2.5.2/jupyterlab_server/settings_handler.py#L216-L227
     */
    function override(plugin) {
        if (_overrides[plugin.id]) {
            if (!plugin.schema.properties) {
                // probably malformed, or only provides keyboard shortcuts, etc.
                plugin.schema.properties = {};
            }
            for (const [prop, propDefault] of Object.entries(_overrides[plugin.id] || {})) {
                plugin.schema.properties[prop].default = propDefault;
            }
        }
        return plugin;
    }
    Private.override = override;
})(Private || (Private = {}));


/***/ }),

/***/ 53439:
/*!*********************************************!*\
  !*** ../../packages/settings/lib/tokens.js ***!
  \*********************************************/
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export */ __webpack_require__.d(__webpack_exports__, {
/* harmony export */   "ISettings": () => (/* binding */ ISettings)
/* harmony export */ });
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @lumino/coreutils */ 9727);
/* harmony import */ var _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(_lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__);

/**
 * The token for the settings service.
 */
const ISettings = new _lumino_coreutils__WEBPACK_IMPORTED_MODULE_0__.Token('@jupyterlite/settings:ISettings');


/***/ })

}]);
//# sourceMappingURL=packages_server-extension_lib_index_js.15d73b8b2ef562a34e9a.js.map