namespace SourceWebserver
{
const char *html_config = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1" />
  <title>ESP32 Configuration</title>
  <style>
    :root{
      --bg1:#74ebd5; --bg2:#ACB6E5;
      --card:#ffffffdd; --text:#222; --muted:#666;
      --primary:#4CAF50; --primary-hover:#45a049;
      --danger:#d9534f;
      --radius:12px;
    }
    *{box-sizing:border-box}
    body{
      margin:0; font-family:system-ui,Segoe UI,Roboto,Arial,sans-serif;
      background:linear-gradient(120deg,var(--bg1),var(--bg2)); color:var(--text);
      min-height:100vh; display:flex; align-items:center; justify-content:center;
      padding:24px;
    }
    .card{
      width:100%; max-width:620px; background:var(--card); border-radius:var(--radius);
      box-shadow:0 10px 30px rgba(0,0,0,.15); padding:24px 22px 28px;
      position:relative; overflow:hidden;
    }
    h2{margin:0 0 6px; text-align:center}
    p.sub{margin:0 0 18px; text-align:center; color:var(--muted); font-size:14px}
    .section{margin-top:18px; padding-top:12px}
    .section h3{margin:0 0 8px; font-size:15px; color:#333}
    .grid{display:grid; grid-template-columns:1fr 1fr; gap:12px}
    @media (max-width:560px){ .grid{grid-template-columns:1fr} }
    label{display:block; font-size:13px; color:#444; margin:6px 0 6px}
    .field{position:relative}
    input[type=text],input[type=password],input[type=number]{
      width:100%; padding:12px 40px 12px 12px; border:1px solid #d0d5dd; border-radius:8px; font-size:14px;
      outline:none; transition:border-color .15s ease;
      background:#fff;
    }
    input:focus{border-color:#94a3b8; box-shadow:0 0 0 3px rgba(148,163,184,.25)}
    .suffix-btn{
      position:absolute; right:8px; top:50%; transform:translateY(-50%);
      border:none; background:#f1f5f9; padding:6px 10px; border-radius:6px; font-size:12px; cursor:pointer;
    }
    .row-actions{display:flex; gap:10px; margin-top:18px}
    button,.btn{
      width:100%; background:var(--primary); color:#fff; padding:12px 14px; border:none; border-radius:8px;
      font-size:15px; cursor:pointer; transition:background .2s ease;
    }
    button:hover,.btn:hover{background:var(--primary-hover)}
    .btn.secondary{background:#0ea5e9}
    .btn.secondary:hover{background:#0284c7}
    .btn.ghost{background:#e2e8f0; color:#111}
    .help{font-size:12px; color:var(--muted); margin-top:6px}
    hr{border:none; border-top:1px solid #e5e7eb; margin:16px 0}
    .spinner{
      position:absolute; inset:0; backdrop-filter:blur(2px); background:rgba(255,255,255,.6);
      display:none; align-items:center; justify-content:center;
    }
    .spinner.show{display:flex}
    .spinner .dot{
      width:10px; height:10px; border-radius:50%; background:#111; margin:0 4px; opacity:.35; animation:b 1s infinite;
    }
    .spinner .dot:nth-child(2){animation-delay:.15s}
    .spinner .dot:nth-child(3){animation-delay:.3s}
    @keyframes b{0%,100%{opacity:.35; transform:translateY(0)}50%{opacity:1; transform:translateY(-6px)}}
    .toast{
      position:fixed; left:50%; transform:translateX(-50%);
      bottom:18px; background:#111; color:#fff; padding:10px 14px; border-radius:8px; font-size:14px; display:none;
    }
    .toast.show{display:block}
  </style>
</head>
<body>
  <div class="card">
    <h2>⚙️ ESP32 Configuration</h2>
    <p class="sub">Cấu hình Wi-Fi, MQTT và các tuỳ chọn khác</p>

    <!-- Form sẽ render động theo FIELDS -->
    <form id="configForm" class="form"></form>

    <div class="row-actions">
      <button class="btn" id="btnSave" type="button">💾 Lưu cấu hình</button>
      <button class="btn secondary" id="btnLoad" type="button">🔍 Tải cấu hình</button>
      <button class="btn ghost" id="btnReset" type="button" title="Chỉ xoá form trên trình duyệt, không xoá NVS">↺ Reset form</button>
    </div>

    <div class="help">Mẹo: Nhấn “Tải cấu hình” để điền sẵn dữ liệu từ thiết bị. Port MQTT hợp lệ: 1–65535.</div>

    <div class="spinner" id="spinner" aria-hidden="true">
      <div class="dot"></div><div class="dot"></div><div class="dot"></div>
    </div>
  </div>

  <div id="toast" class="toast" role="status" aria-live="polite"></div>

<script>
/* =======================
   KHAI BÁO TRƯỜNG DẠNG KHAI BÁO – DỄ THÊM MỚI
   Chỉ cần thêm object vào FIELDS, form sẽ tự render.
   ======================= */
const FIELDS = [
  { section: "Wi-Fi", items: [
    { id:"ssid",     name:"ssid",     label:"📶 Wi-Fi SSID",      type:"text",     required:true,  placeholder:"MyWiFi" },
    { id:"pass",     name:"pass",     label:"🔑 Wi-Fi Password",  type:"password", required:false, placeholder:"••••••", toggle:true }
  ]},
  { section: "MQTT", items: [
    { id:"uriMqtt",  name:"uriMqtt",  label:"🔗 MQTT URI",        type:"text",     required:true,  placeholder:"mqtt://broker.local" },
    { id:"portMqtt", name:"portMqtt", label:"🔌 MQTT Port",       type:"number",   required:true,  min:1, max:65535, step:1, value:1883, inputmode:"numeric" },
    { id:"mqttUser", name:"mqttUser", label:"👤 MQTT Username",   type:"text",     required:false, placeholder:"(optional)" },
    { id:"mqttPass", name:"mqttPass", label:"🔒 MQTT Password",   type:"password", required:false, placeholder:"••••••", toggle:true }
  ]}
  // Thêm section/field mới ở đây nếu cần
];

const $ = (id)=>document.getElementById(id);

function showToast(msg, ms=1800){
  const t=$('toast'); t.textContent=msg; t.classList.add('show');
  setTimeout(()=>t.classList.remove('show'), ms);
}
function setBusy(b){ $('spinner').classList.toggle('show', b); }
function clampPort(v){ v = Number(v)||0; if (v<1) v=1; if (v>65535) v=65535; return v; }

/* Render form theo FIELDS */
function renderForm(){
  const form = $('configForm');
  form.innerHTML = "";
  FIELDS.forEach(group=>{
    const sec = document.createElement('div');
    sec.className = 'section';
    sec.innerHTML = `<h3>${group.section}</h3>`;
    const grid = document.createElement('div');
    grid.className = 'grid';
    group.items.forEach(f=>{
      const wrap = document.createElement('div');
      wrap.className = 'field';
      const label = document.createElement('label');
      label.setAttribute('for', f.id);
      label.textContent = f.label;
      const input = document.createElement('input');
      input.id = f.id; input.name = f.name; input.type = f.type || 'text';
      if (f.placeholder) input.placeholder = f.placeholder;
      if (f.required) input.required = true;
      if (f.value!==undefined) input.value = f.value;
      if (f.min!==undefined) input.min = f.min;
      if (f.max!==undefined) input.max = f.max;
      if (f.step!==undefined) input.step = f.step;
      if (f.inputmode) input.setAttribute('inputmode', f.inputmode);
      // toggle password
      if (f.type === 'password' && f.toggle){
        const btn = document.createElement('button');
        btn.type='button'; btn.className='suffix-btn'; btn.textContent='Hiện';
        btn.addEventListener('click', ()=>{
          input.type = (input.type==='password') ? 'text' : 'password';
          btn.textContent = (input.type==='password') ? 'Hiện' : 'Ẩn';
        });
        wrap.appendChild(btn);
      }
      wrap.appendChild(label);
      wrap.appendChild(input);
      grid.appendChild(wrap);
    });
    sec.appendChild(grid);
    form.appendChild(sec);
  });
}

/* Nạp cấu hình từ thiết bị */
async function loadConfig(){
  setBusy(true);
  try{
    const res = await fetch('/get-config', { method:'GET' });
    if(!res.ok) throw new Error('HTTP '+res.status);
    const data = await res.json();

    // Gán giá trị an toàn
    const setVal = (id, v)=>{ const el=$(id); if(el) el.value = (v??""); };
    setVal('ssid', data.ssid);
    setVal('pass', data.pass);
    setVal('uriMqtt', data.uriMqtt);
    setVal('portMqtt', data.portMqtt);
    setVal('mqttUser', data.mqttUser);
    setVal('mqttPass', data.mqttPass);

    showToast('Đã tải cấu hình ✅');
  }catch(e){
    console.error(e);
    showToast('Không tải được cấu hình ❌');
    alert("❌ Failed to load config: "+e);
  }finally{
    setBusy(false);
  }
}

/* Lưu cấu hình lên thiết bị */
async function saveConfig(){
  const form = $('configForm');
  if (!form.reportValidity()){ return; }

  const fd = new FormData(form);
  // Chuẩn hoá giá trị port
  const port = clampPort(fd.get('portMqtt'));
  fd.set('portMqtt', port);

  const body = new URLSearchParams(fd).toString();

  setBusy(true);
  try{
    const res = await fetch('/config', {
      method:'POST',
      headers:{ 'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8' },
      body
    });
    if(!res.ok) throw new Error('HTTP '+res.status);
    showToast('Lưu thành công ✅');
    alert("✅ Configuration saved successfully!");
  }catch(e){
    console.error(e);
    showToast('Lưu thất bại ❌');
    alert("❌ Failed to save configuration! " + e);
  }finally{
    setBusy(false);
  }
}

/* Reset form (trên trình duyệt) */
function resetForm(){
  $('configForm').reset();
  showToast('Đã reset form');
}

/* Khởi tạo */
document.addEventListener('DOMContentLoaded', ()=>{
  renderForm();
  $('btnLoad').addEventListener('click', loadConfig);
  $('btnSave').addEventListener('click', saveConfig);
  $('btnReset').addEventListener('click', resetForm);
});
</script>
</body>
</html>
)rawliteral";
}
