const apiHost = 'localhost';
const apiPort = '4435';

export async function makeRequest(host, port, url, method, token, data) {
    const requestData = {
        method: method,
        headers: {},
        body: null
    };

    if (token) {
        requestData.headers['Authorization'] = 'HCAuth Type=Client Token=' + token;
    }

    if (data) {
        requestData.headers['Content-Type'] = 'application/json';
        requestData.body = JSON.stringify(data);
    }
    
    let response = null;
    try {
        response = await fetch('https://' + host + ':' + port + url, requestData);
    } catch(err) {
        throw 'failed to connect to api';
    }

    let responseJson = null;
    try {
        responseJson = await response.json();
    } catch(err) {
        throw 'failed to parse json from api';
    }

    return responseJson;
}

export async function login(username, password) {
    const response = await makeRequest(apiHost, apiPort, '/login', 'POST', null, {
        username: username,
        password: password
    });

    if (!response.success || !response.token) {
        throw 'login failed';
    }

    return response;
}

export async function fetchDevices(token) {
    const response = await makeRequest(apiHost, apiPort, '/device', 'GET', token, null);

    if (!response.success || !response.nodes || !response.devices) {
        throw 'fetch devices failed';
    }

    return response;
}

export async function nodeConnect(nodeHost, nodePort, token) {
    const response = await makeRequest(nodeHost, nodePort, '/connect/user', 'POST', token, null);

    if (!response.success || !response.ticket) {
        throw 'node connect failed';
    }

    return response;
}