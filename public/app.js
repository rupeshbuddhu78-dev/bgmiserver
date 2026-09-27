const result = document.querySelector('#result');
document.querySelector('#check').addEventListener('click', async () => {
  result.textContent = 'Checking...';
  try {
    const response = await fetch('/api/validate-key', { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({ key: document.querySelector('#key').value, appId: 'education-demo', deviceId: document.querySelector('#device').value }) });
    result.textContent = JSON.stringify(await response.json(), null, 2);
  } catch (error) { result.textContent = error.message; }
});
