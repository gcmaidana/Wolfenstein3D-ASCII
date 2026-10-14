const http=require('node:http');
const fs=require('node:fs');
const path=require('node:path');
const packaged=path.resolve(__dirname,'../dist/web');
const root=fs.existsSync(path.join(packaged,'wolf-ascii.wasm'))?packaged:path.resolve(__dirname,'../docs');
if(!fs.existsSync(path.join(root,'wolf-ascii.wasm'))){console.error('Build/package the web version first. See README.md.');process.exit(1);}
http.createServer((req,res)=>{
 const name=req.url.split('?')[0];
 const filename=path.join(root,name==='/'?'index.html':path.basename(name));
 if(!fs.existsSync(filename)||!fs.statSync(filename).isFile()){res.writeHead(404).end('Not found');return;}
 const mime={'.html':'text/html; charset=utf-8','.js':'application/javascript','.wasm':'application/wasm','.zip':'application/zip','.md':'text/plain; charset=utf-8'};
 res.setHeader('Content-Type',mime[path.extname(filename)]||'application/octet-stream');
 fs.createReadStream(filename).pipe(res);
}).listen(8080,'127.0.0.1',()=>console.log('Open http://127.0.0.1:8080. Episode 1 boots automatically. Press Ctrl+C to stop.'))
.on('error',error=>{console.error(error.message);process.exitCode=1;});
