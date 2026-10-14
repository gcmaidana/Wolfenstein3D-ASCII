// Integration check: serve the build, load real data, start a level and exercise display modes.
const {chromium}=require('playwright');
const fs=require('node:fs');
const path=require('node:path');
const http=require('node:http');
const assert=require('node:assert/strict');
const root=path.resolve(__dirname,'..');
const web=path.join(root,'dist/web');
const server=http.createServer((req,res)=>{
 const name=req.url.split('?')[0];
 const file=path.join(web,name==='/'?'index.html':path.basename(name));
 if(!fs.existsSync(file)){res.writeHead(404).end();return;}
 res.setHeader('Content-Type',file.endsWith('.wasm')?'application/wasm':file.endsWith('.js')?'application/javascript':'text/html');
 fs.createReadStream(file).pipe(res);
});
(async()=>{
 await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
 const browser=await chromium.launch({channel:'chrome',headless:true});
 try{
  const page=await browser.newPage({viewport:{width:1100,height:1100}});
  const errors=[];page.on('pageerror',e=>errors.push(e.message));
  page.on('console',m=>{if(m.type()==='error')console.error(m.text());});
  await page.goto(`http://127.0.0.1:${server.address().port}/`);
  await page.waitForFunction(()=>document.getElementById('loader').hidden,{},{timeout:30000});
  assert.equal(await page.locator('input[type=file]').count(),0,'Demo needs no file selection');
  assert.equal(await page.locator('#start').count(),0,'Main menu boots automatically');
  await page.waitForTimeout(2500);
  await page.screenshot({path:path.join(root,'build-web/menu.png')});
  await page.keyboard.down('v');await page.waitForTimeout(120);await page.keyboard.up('v');await page.waitForTimeout(300);
  await page.keyboard.down('Enter');await page.waitForTimeout(120);await page.keyboard.up('Enter');await page.waitForTimeout(900);
  await page.screenshot({path:path.join(root,'build-web/scores.png')});
  await page.keyboard.down('Escape');await page.waitForTimeout(120);await page.keyboard.up('Escape');await page.waitForTimeout(900);
  await page.keyboard.down('n');await page.waitForTimeout(120);await page.keyboard.up('n');await page.waitForTimeout(300);
  // New Game goes directly to Episode 1 difficulty; cancel returns to main.
  await page.keyboard.down('Enter');await page.waitForTimeout(120);await page.keyboard.up('Enter');await page.waitForTimeout(900);
  await page.screenshot({path:path.join(root,'build-web/difficulty.png')});
  await page.keyboard.down('Escape');await page.waitForTimeout(120);await page.keyboard.up('Escape');await page.waitForTimeout(900);
  await page.screenshot({path:path.join(root,'build-web/cancel-new-game.png')});
  for(let i=0;i<2;i++){await page.keyboard.down('Enter');await page.waitForTimeout(120);await page.keyboard.up('Enter');await page.waitForTimeout(i===1?350:900);}
  await page.screenshot({path:path.join(root,'build-web/loading.png')});
  await page.waitForTimeout(3000);
  assert(await page.evaluate(()=>Module._WA_IsPlaying()),'Must enter a real level, not just animate a menu');
  await page.screenshot({path:path.join(root,'build-web/gameplay.png')});
  const before=await page.locator('#canvas').screenshot();
  await page.keyboard.down('ArrowUp');await page.waitForTimeout(600);await page.keyboard.up('ArrowUp');
  const after=await page.locator('#canvas').screenshot();
  assert(!before.equals(after),'Input should change the frame');
  await page.getByRole('button',{name:'Green ASCII',exact:true}).click();
  await page.getByRole('button',{name:'Detail: 80 × 50',exact:true}).click();
  await page.waitForTimeout(200);
  await page.screenshot({path:path.join(root,'build-web/detail.png')});
  await page.getByRole('button',{name:'Original pixels',exact:true}).click();
  await page.waitForTimeout(200);
  await page.screenshot({path:path.join(root,'build-web/original.png')});
  await page.getByRole('button',{name:'Color ASCII',exact:true}).click();
  await page.keyboard.down('Escape');await page.waitForTimeout(120);await page.keyboard.up('Escape');
  await page.waitForTimeout(900);
  assert(!(await page.evaluate(()=>Module._WA_IsPlaying())),'Escape opens the menu');
  await page.screenshot({path:path.join(root,'build-web/pause-menu.png')});
  // The original menu selects "Back to Game"; Escape opens its quit dialog.
  await page.keyboard.down('Enter');await page.waitForTimeout(120);await page.keyboard.up('Enter');
  await page.waitForTimeout(900);
  assert(await page.evaluate(()=>Module._WA_IsPlaying()),'Back to Game resumes gameplay');
  await page.screenshot({path:path.join(root,'build-web/resumed.png')});
  const heap=await page.evaluate(()=>HEAPU8.buffer.byteLength);
  assert(heap<=128*1024*1024,'Memory cap');
  assert.deepEqual(errors,[],'No browser runtime exceptions');
  console.log(JSON.stringify({result:'PASS',wasmHeapMiB:heap/1048576,browser:await browser.version(),screenshots:'build-web/*.png'}));
 }finally{await browser.close();server.close();}
})().catch(error=>{console.error(error);server.close();process.exitCode=1;});
