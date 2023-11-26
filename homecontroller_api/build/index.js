"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const https = require("https");
const express = require("express");
const cors = require("cors");
const logger_1 = require("./util/logger");
const response_1 = require("./response");
const auth_manager_1 = require("./auth_manager");
const user_manager_1 = require("./user_manager");
const ticket_service_1 = require("./ticket_service");
const authManager = new auth_manager_1.default();
const userManager = new user_manager_1.default();
const ticketService = new ticket_service_1.default();
const certFile = fs.readFileSync('cert/fullchain.pem');
const privKeyFile = fs.readFileSync('cert/privkey.pem');
const app = express();
app.use(express.json());
app.use(cors());
app.post('/login/user', (req, res) => {
    if (!req.body.username || !req.body.password) {
        res.status(400);
        res.json((0, response_1.errorResponse)('bad request', 600));
        res.end();
        return;
    }
    try {
        let user = userManager.getUser(req.body.username, req.body.password);
        let token = authManager.createSession(user.id);
        let response = {
            success: true,
            userId: user.id,
            username: user.username,
            nodes: [],
            devices: Array.from(user.devices.values()),
            token: token
        };
        for (const [key, value] of user.nodes.entries()) {
            let ticket = ticketService.createUserTicket(user.id, key);
            response.nodes.push({
                node: value,
                ticket: ticket
            });
        }
        logger_1.Logger.log('created session [' + response.token + '] for user [' + response.username + ']');
        res.status(200);
        res.json(response);
    }
    catch (e) {
        res.status(400);
        res.json((0, response_1.errorResponse)('login failed', e.getErrorCode()));
    }
    res.end();
});
app.post('/login/device', (req, res) => {
    if (!req.body.username || !req.body.password || !req.body.deviceId) {
        res.status(400);
        res.json((0, response_1.errorResponse)('bad request', 600));
        res.end();
        return;
    }
    try {
        let user = userManager.getUser(req.body.username, req.body.password);
        if (!user.devices.has(req.body.deviceId)) {
            res.status(400);
            res.json((0, response_1.errorResponse)('device not found', 800));
            res.end();
            return;
        }
        let device = user.devices.get(req.body.deviceId);
        if (!user.nodes.has(device.nodeId)) {
            res.status(400);
            res.json((0, response_1.errorResponse)('node not found', 900));
            res.end();
            return;
        }
        let node = user.nodes.get(device.nodeId);
        let ticket = ticketService.createDeviceTicket(user.id, device.id, node.id);
        let token = authManager.createSession(user.id);
        let response = {
            success: true,
            userId: user.id,
            deviceId: device.id,
            node: node,
            ticket: ticket,
            token: token
        };
        logger_1.Logger.log('created session [' + response.token + '] for device [' + response.deviceId + ']');
        res.status(200);
        res.json(response);
    }
    catch (e) {
        res.status(400);
        res.json((0, response_1.errorResponse)('login failed', e.getErrorCode()));
    }
    res.end();
});
app.post('/register_device', (req, res) => {
});
app.post('/reconnect/user', (req, res) => {
});
app.post('/reconnect/device', (req, res) => {
});
app.post('/validate_user', (req, res) => {
    let nodeId;
    try {
        let nodeSecret = authManager.parseAuthHeader(req);
        nodeId = authManager.validateNode(nodeSecret);
    }
    catch (e) {
        res.status(401);
        res.json((0, response_1.errorResponse)('unauthorized', 700));
        res.end();
        return;
    }
    if (!req.body.ticket) {
        res.status(400);
        res.json((0, response_1.errorResponse)('bad request', 600));
        res.end();
        return;
    }
    try {
        let ticketData = ticketService.checkUserTicket(req.body.ticket, nodeId);
        let response = {
            success: true,
            userId: ticketData.userId
        };
        res.status(200);
        res.json(response);
    }
    catch (e) {
        res.status(400);
        res.json((0, response_1.errorResponse)('validate user failed', e.getErrorCode()));
    }
    res.end();
});
app.post('/validate_device', (req, res) => {
    let nodeId;
    try {
        let nodeSecret = authManager.parseAuthHeader(req);
        nodeId = authManager.validateNode(nodeSecret);
    }
    catch (e) {
        res.status(401);
        res.json((0, response_1.errorResponse)('unauthorized', 700));
        res.end();
        return;
    }
    if (!req.body.ticket) {
        res.status(400);
        res.json((0, response_1.errorResponse)('bad request', 600));
        res.end();
        return;
    }
    try {
        let ticketData = ticketService.checkDeviceTicket(req.body.ticket, nodeId);
        let response = {
            success: true,
            userId: ticketData.userId,
            deviceId: ticketData.deviceId
        };
        res.status(200);
        res.json(response);
    }
    catch (e) {
        res.status(400);
        res.json((0, response_1.errorResponse)('validate device failed', e.getErrorCode()));
    }
    res.end();
});
app.get('/device', (req, res) => {
});
// 404 not found
/*app.use((req, res, next) => {
    res.status(404);
    res.json(errorResponse('module not found', 500));
    res.end();
});*/
const port = 4435;
const credentials = {
    key: privKeyFile,
    cert: certFile
};
logger_1.Logger.enableDebug();
const httpsServer = https.createServer(credentials, app);
httpsServer.keepAliveTimeout = 30000;
httpsServer.listen(port);
logger_1.Logger.log('server listening on port ' + port);
