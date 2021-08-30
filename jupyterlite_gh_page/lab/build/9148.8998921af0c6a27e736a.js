(self.webpackChunk_JUPYTERLAB_CORE_OUTPUT=self.webpackChunk_JUPYTERLAB_CORE_OUTPUT||[]).push([[9148],{39148:(t,s,e)=>{"use strict";e.r(s),e.d(s,{JupyterLiteServer:()=>i,JupyterServer:()=>a,LiteServiceManager:()=>u});var n=e(12131);class i extends n.Application{constructor(t){super(t),this.name="JupyterLite Server",this.namespace=this.name,this.version="unknown",this._serviceManager=null}get serviceManager(){return this._serviceManager}attachShell(t){}evtResize(t){}registerPluginModule(t){let s=t.default;Object.prototype.hasOwnProperty.call(t,"__esModule")||(s=t),Array.isArray(s)||(s=[s]),s.forEach((t=>{try{this.registerPlugin(t)}catch(t){console.error(t)}}))}registerPluginModules(t){t.forEach((t=>{this.registerPluginModule(t)}))}registerServiceManager(t){this._serviceManager=t}}class r{constructor(){this._routes=[]}get(t,s){this._add("GET",t,s)}put(t,s){this._add("PUT",t,s)}post(t,s){this._add("POST",t,s)}patch(t,s){this._add("PATCH",t,s)}delete(t,s){this._add("DELETE",t,s)}async route(t){const s=new URL(t.url),{method:e}=t,{pathname:n}=s;for(const i of this._routes){if(i.method!==e)continue;const r=n.match(i.pattern);if(!r)continue;const a=r.slice(1);let o;if("PATCH"===i.method||"PUT"===i.method||"POST"===i.method)try{o=JSON.parse(await t.text())}catch{o=void 0}return i.callback.call(null,{pathname:n,body:o,query:Object.fromEntries(s.searchParams)},...a)}throw new Error("Cannot route "+t.method+" "+t.url)}_add(t,s,e){"string"==typeof s&&(s=new RegExp(s)),this._routes.push({method:t,pattern:s,callback:e})}}class a{constructor(t){this._router=new r;const{contents:s,kernels:e,kernelspecs:n,sessions:i,settings:a}=t;this._contents=s,this._kernels=e,this._kernelspecs=n,this._sessions=i,this._settings=a,this._addRoutes()}async fetch(t,s){if(!(t instanceof Request))throw Error("Request info is not a Request");return this._router.route(t)}_addRoutes(){const t=this._router;t.get("/api/contents/(.+)/checkpoints",(async(t,s)=>{const e=await this._contents.listCheckpoints(s);return new Response(JSON.stringify(e))})),t.post("/api/contents/(.+)/checkpoints/(.*)",(async(t,s,e)=>{const n=await this._contents.restoreCheckpoint(s,e);return new Response(JSON.stringify(n),{status:204})})),t.post("/api/contents/(.+)/checkpoints",(async(t,s)=>{const e=await this._contents.createCheckpoint(s);return new Response(JSON.stringify(e),{status:201})})),t.delete("/api/contents/(.+)/checkpoints/(.*)",(async(t,s,e)=>{const n=await this._contents.deleteCheckpoint(s,e);return new Response(JSON.stringify(n),{status:204})})),t.get("/api/contents(.*)",(async(t,s)=>{var e;const n={content:"1"===(null===(e=t.query)||void 0===e?void 0:e.content)},i=await this._contents.get(s,n);return new Response(JSON.stringify(i))})),t.post("/api/contents(.*)",(async(t,s)=>{const e=t.body,n=null==e?void 0:e.copy_from;let i;return i=n?await this._contents.copy(n,s):await this._contents.newUntitled(e),new Response(JSON.stringify(i),{status:201})})),t.patch("/api/contents(.*)",(async(t,s)=>{var e,n;const i=null!==(n=null===(e=t.body)||void 0===e?void 0:e.path)&&void 0!==n?n:"";s="/"===s[0]?s.slice(1):s;const r=await this._contents.rename(s,i);return new Response(JSON.stringify(r))})),t.put("/api/contents/(.+)",(async(t,s)=>{const e=t.body,n=await this._contents.save(s,e);return new Response(JSON.stringify(n))})),t.delete("/api/contents/(.+)",(async(t,s)=>(await this._contents.delete(s),new Response(null,{status:204})))),t.post("/api/kernels/(.*)/restart",(async(t,s)=>{const e=await this._kernels.restart(s);return new Response(JSON.stringify(e))})),t.delete("/api/kernels/(.*)",(async(t,s)=>{const e=await this._kernels.shutdown(s);return new Response(JSON.stringify(e),{status:204})})),t.get("/api/kernelspecs",(async t=>{const s=this._kernelspecs.specs;return new Response(JSON.stringify(s))})),t.get("/api/nbconvert",(async t=>new Response(JSON.stringify({})))),t.get("/api/sessions/(.+)",(async(t,s)=>{const e=await this._sessions.get(s);return new Response(JSON.stringify(e),{status:200})})),t.get("/api/sessions",(async t=>{const s=await this._sessions.list();return new Response(JSON.stringify(s),{status:200})})),t.patch("/api/sessions(.*)",(async(t,s)=>{const e=t.body,n=await this._sessions.patch(e);return new Response(JSON.stringify(n),{status:200})})),t.delete("/api/sessions/(.+)",(async(t,s)=>(await this._sessions.shutdown(s),new Response(null,{status:204})))),t.post("/api/sessions",(async t=>{const s=t.body,e=await this._sessions.startNew(s);return new Response(JSON.stringify(e),{status:201})}));const s="/api/settings/((?:@([^/]+?)[/])?([^/]+?):([^:]+))$";t.get(s,(async(t,s)=>{const e=await this._settings.get(s);return new Response(JSON.stringify(e))})),t.put(s,(async(t,s)=>{const e=t.body,{raw:n}=e;return await this._settings.save(s,n),new Response(null,{status:204})})),t.get("/api/settings",(async t=>{const s=await this._settings.getAll();return new Response(JSON.stringify(s))}))}}var o=e(56624),c=e(79988);class u extends o.ServiceManager{constructor(t){var s;const e=t.server;super({...t,serverSettings:{...o.ServerConnection.makeSettings(),WebSocket:c.WebSocket,fetch:null!==(s=e.fetch.bind(e))&&void 0!==s?s:void 0}}),this._server=e}get server(){return this._server}}}}]);
//# sourceMappingURL=9148.8998921af0c6a27e736a.js.map