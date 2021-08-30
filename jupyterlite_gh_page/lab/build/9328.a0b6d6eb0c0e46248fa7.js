(self.webpackChunk_JUPYTERLAB_CORE_OUTPUT=self.webpackChunk_JUPYTERLAB_CORE_OUTPUT||[]).push([[9328],{65246:(e,s,t)=>{"use strict";t.d(s,{I:()=>r});var a=t(75357),n=t(75138);class r{constructor(e){this._history=[],this._executionCount=0,this._isDisposed=!1,this._disposed=new n.Signal(this),this._parentHeader=void 0,this._parent=void 0;const{id:s,name:t,sendMessage:a}=e;this._id=s,this._name=t,this._sendMessage=a}get ready(){return Promise.resolve()}get isDisposed(){return this._isDisposed}get disposed(){return this._disposed}get id(){return this._id}get name(){return this._name}get executionCount(){return this._executionCount}get parentHeader(){return this._parentHeader}get parent(){return this._parent}dispose(){this.isDisposed||(this._isDisposed=!0,this._disposed.emit(void 0))}async handleMessage(e){switch(this._busy(e),this._parent=e,e.header.msg_type){case"kernel_info_request":await this._kernelInfo(e);break;case"execute_request":await this._execute(e);break;case"input_reply":this.inputReply(e.content);break;case"inspect_request":await this._inspect(e);break;case"is_complete_request":await this._isCompleteRequest(e);break;case"complete_request":await this._complete(e);break;case"history_request":await this._historyRequest(e);break;case"comm_open":await this.commOpen(e);break;case"comm_msg":await this.commMsg(e);break;case"comm_close":await this.commClose(e)}this._idle(e)}stream(e){var s,t;const n=a.KernelMessage.createMessage({channel:"iopub",msgType:"stream",session:null!==(t=null===(s=this._parentHeader)||void 0===s?void 0:s.session)&&void 0!==t?t:"",parentHeader:this._parentHeader,content:e});this._sendMessage(n)}displayData(e){var s,t,n;e.metadata=null!==(s=e.metadata)&&void 0!==s?s:{};const r=a.KernelMessage.createMessage({channel:"iopub",msgType:"display_data",session:null!==(n=null===(t=this._parentHeader)||void 0===t?void 0:t.session)&&void 0!==n?n:"",parentHeader:this._parentHeader,content:e});this._sendMessage(r)}inputRequest(e){var s,t;const n=a.KernelMessage.createMessage({channel:"stdin",msgType:"input_request",session:null!==(t=null===(s=this._parentHeader)||void 0===s?void 0:s.session)&&void 0!==t?t:"",parentHeader:this._parentHeader,content:e});this._sendMessage(n)}publishExecuteResult(e){var s,t;const n=a.KernelMessage.createMessage({channel:"iopub",msgType:"execute_result",session:null!==(t=null===(s=this._parentHeader)||void 0===s?void 0:s.session)&&void 0!==t?t:"",parentHeader:this._parentHeader,content:e});this._sendMessage(n)}publishExecuteError(e){var s,t;const n=a.KernelMessage.createMessage({channel:"iopub",msgType:"error",session:null!==(t=null===(s=this._parentHeader)||void 0===s?void 0:s.session)&&void 0!==t?t:"",parentHeader:this._parentHeader,content:e});this._sendMessage(n)}updateDisplayData(e){var s,t;const n=a.KernelMessage.createMessage({channel:"iopub",msgType:"update_display_data",session:null!==(t=null===(s=this._parentHeader)||void 0===s?void 0:s.session)&&void 0!==t?t:"",parentHeader:this._parentHeader,content:e});this._sendMessage(n)}clearOutput(e){var s,t;const n=a.KernelMessage.createMessage({channel:"iopub",msgType:"clear_output",session:null!==(t=null===(s=this._parentHeader)||void 0===s?void 0:s.session)&&void 0!==t?t:"",parentHeader:this._parentHeader,content:e});this._sendMessage(n)}handleComm(e,s,t,n){var r,i;const o=a.KernelMessage.createMessage({channel:"iopub",msgType:e,session:null!==(i=null===(r=this._parentHeader)||void 0===r?void 0:r.session)&&void 0!==i?i:"",parentHeader:this._parentHeader,content:s,metadata:t,buffers:n});this._sendMessage(o)}_idle(e){const s=a.KernelMessage.createMessage({msgType:"status",session:e.header.session,parentHeader:e.header,channel:"iopub",content:{execution_state:"idle"}});this._sendMessage(s)}_busy(e){const s=a.KernelMessage.createMessage({msgType:"status",session:e.header.session,parentHeader:e.header,channel:"iopub",content:{execution_state:"busy"}});this._sendMessage(s)}async _kernelInfo(e){const s=await this.kernelInfoRequest(),t=a.KernelMessage.createMessage({msgType:"kernel_info_reply",channel:"shell",session:e.header.session,parentHeader:e.header,content:s});this._sendMessage(t)}async _historyRequest(e){const s=e,t=a.KernelMessage.createMessage({msgType:"history_reply",channel:"shell",parentHeader:s.header,session:e.header.session,content:{status:"ok",history:this._history}});this._sendMessage(t)}_executeInput(e){const s=e,t=s.content.code,n=a.KernelMessage.createMessage({msgType:"execute_input",parentHeader:s.header,channel:"iopub",session:e.header.session,content:{code:t,execution_count:this._executionCount}});this._sendMessage(n)}async _execute(e){const s=e,t=s.content;this._executionCount++,this._parentHeader=s.header,this._executeInput(s),this._history.push([0,0,t.code]);const n=await this.executeRequest(s.content),r=a.KernelMessage.createMessage({msgType:"execute_reply",channel:"shell",parentHeader:s.header,session:e.header.session,content:n});this._sendMessage(r)}async _complete(e){const s=e,t=await this.completeRequest(s.content),n=a.KernelMessage.createMessage({msgType:"complete_reply",parentHeader:s.header,channel:"shell",session:e.header.session,content:t});this._sendMessage(n)}async _inspect(e){const s=e,t=await this.inspectRequest(s.content),n=a.KernelMessage.createMessage({msgType:"inspect_reply",parentHeader:s.header,channel:"shell",session:e.header.session,content:t});this._sendMessage(n)}async _isCompleteRequest(e){const s=e,t=await this.isCompleteRequest(s.content),n=a.KernelMessage.createMessage({msgType:"is_complete_reply",parentHeader:s.header,channel:"shell",session:e.header.session,content:t});this._sendMessage(n)}}},64831:(e,s,t)=>{"use strict";t.d(s,{L:()=>n,q:()=>r});var a=t(9727);const n=new a.Token("@jupyterlite/kernel:IKernels"),r=new a.Token("@jupyterlite/kernelspec:IKernelSpecs")},39328:(e,s,t)=>{"use strict";t.r(s),t.d(s,{default:()=>c});var a=t(64831),n=t(65246),r=t(9727);function i(){return new Worker(t.p+"bundle.worker.js")}class o extends n.I{constructor(e){super(e),this._executeDelegate=new r.PromiseDelegate,this._worker=new i,this._worker.onmessage=e=>{this._processWorkerMessage(e.data)}}dispose(){this.isDisposed||super.dispose()}_processWorkerMessage(e){var s,t,a,n,r,i,o;switch(e.type){case"stream":{const t=null!==(s=e.bundle)&&void 0!==s?s:{name:"stdout",text:""};this.stream(t);break}case"input_request":{console.log("input-request",e.content);const s=null!==(t=e.content)&&void 0!==t?t:{prompt:"",password:!1};this.inputRequest(s);break}case"reply":{const s=e.results;this._executeDelegate.resolve(s);break}case"display_data":{const s=null!==(a=e.bundle)&&void 0!==a?a:{data:{},metadata:{},transient:{}};this.displayData(s);break}case"update_display_data":{const s=null!==(n=e.bundle)&&void 0!==n?n:{data:{},metadata:{},transient:{}};this.updateDisplayData(s);break}case"clear_output":{const s=null!==(r=e.bundle)&&void 0!==r?r:{wait:!1};this.clearOutput(s);break}case"execute_result":{const s=null!==(i=e.bundle)&&void 0!==i?i:{execution_count:0,data:{},metadata:{}};this.publishExecuteResult(s);break}case"execute_error":{const s=null!==(o=e.bundle)&&void 0!==o?o:{ename:"",evalue:"",traceback:[]};this.publishExecuteError(s);break}case"comm_msg":case"comm_open":case"comm_close":this.handleComm(e.type,e.content,e.metadata,e.buffers);break;default:this._executeDelegate.resolve({data:{},metadata:{}})}}async kernelInfoRequest(){return{implementation:"xeus",implementation_version:"0.1.0",language_info:{codemirror_mode:{name:"lua",version:3},file_extension:".lua",mimetype:"text/x-lua",name:"lua",nbconvert_exporter:"lua",pygments_lexer:"lua",version:"5.3"},protocol_version:"5.3",status:"ok",banner:"Xeus: A WebAssembly-powered dummy kernel backed by xeus",help_links:[{text:"Lua (WASM) Kernel",url:"https://xeus.org"}]}}async executeRequest(e){const s=await this._sendRequestMessageToWorker("execute-request",e);return{execution_count:this.executionCount,...s}}async completeRequest(e){return await this._sendRequestMessageToWorker("complete-request",e)}async inspectRequest(e){return await this._sendRequestMessageToWorker("inspect-request",e)}async isCompleteRequest(e){return await this._sendRequestMessageToWorker("is-complete-request",e)}async commInfoRequest(e){return await this._sendRequestMessageToWorker("comm-info-request",e)}inputReply(e){this._worker.postMessage({type:"input-reply",data:e,parent:this.parent})}async commOpen(e){return await this._sendRequestMessageToWorker("comm-open",e)}async commMsg(e){return await this._sendRequestMessageToWorker("comm-msg",e)}async commClose(e){return await this._sendRequestMessageToWorker("comm-close",e)}async _sendRequestMessageToWorker(e,s){return this._executeDelegate=new r.PromiseDelegate,this._worker.postMessage({type:e,data:s,parent:this.parent}),await this._executeDelegate.promise}}const c=[{id:"@jupyterlite/xeus-kernel-extension:kernel",autoStart:!0,requires:[a.q],activate:(e,s)=>{s.register({spec:{name:"Xeus-Lua",display_name:"Xeus-Lua",language:"lua",argv:[],spec:{argv:[],env:{},display_name:"Xeus-Lua",language:"lua",interrupt_mode:"message",metadata:{}},resources:{"logo-32x32":"TODO","logo-64x64":"/kernelspecs/xeus_small.svg"}},create:async e=>new o({...e})})}}]}}]);
//# sourceMappingURL=9328.a0b6d6eb0c0e46248fa7.js.map