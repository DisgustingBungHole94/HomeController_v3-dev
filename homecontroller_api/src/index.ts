import * as fs from 'fs';
import * as https from 'https';

import * as express from 'express';
import * as cors from 'cors';

import { Logger } from './util/logger';
import Exception from './util/exception';
import { 
            errorResponse,
            NodeTicket,
            LoginUserResponse, 
            LoginDeviceResponse, 
            ValidateUserResponse, 
            ValidateDeviceResponse, 
            ConnectUserResponse
        } from './response'
import AuthManager from './auth_manager';
import { AuthType } from './auth_manager';
import UserManager from './user_manager';
import { User, Device, Node } from './user_manager';
import TicketService from './ticket_service';
import { UserTicketData, DeviceTicketData } from './ticket_service';

const authManager: AuthManager = new AuthManager();
const userManager: UserManager = new UserManager();
const ticketService: TicketService = new TicketService();

const certFile: Buffer = fs.readFileSync('cert/fullchain.pem');
const privKeyFile: Buffer = fs.readFileSync('cert/privkey.pem');

const app: any = express();
app.use(express.json());
app.use(cors());

app.post('/login/user', (req, res) => {
    if (!req.body.username || !req.body.password) {
        res.status(400);
        res.json(errorResponse('bad request', 600));
        res.end();
        return;
    }

    try {
        let user: User = userManager.getUser(req.body.username, req.body.password);
        let token: string = authManager.createSession(user.id);

        let response: LoginUserResponse = {
            success: true,
            userId: user.id,
            username: user.username,
            //nodes: [],
            //devices: Array.from(user.devices.values()),
            token: token
        };

        /*for (const [key, value] of user.nodes.entries()) {
            let ticket = ticketService.createUserTicket(user.id, key);
            response.nodes.push({
                node: value,
                ticket: ticket
            });
        }*/

        Logger.log('created session [' + response.token + '] for user [' + response.username + ']');

        res.status(200);
        res.json(response);
    } catch(e) {
        res.status(400);
        res.json(errorResponse('login failed', e.getErrorCode()));
    }

    res.end();
});

app.post('/login/device', (req, res) => {
    if (!req.body.username || !req.body.password || !req.body.deviceId) {
        res.status(400);
        res.json(errorResponse('bad request', 600));
        res.end();
        return;
    }

    try {
        let user: User = userManager.getUser(req.body.username, req.body.password);

        if (!user.devices.has(req.body.deviceId)) {
            res.status(400);
            res.json(errorResponse('device not found', 800));
            res.end();
            return;
        }
        let device: Device = user.devices.get(req.body.deviceId)!;

        if (!user.nodes.has(device.nodeId)) {
            res.status(400);
            res.json(errorResponse('node not found', 900));
            res.end();
            return;
        }
        let node: Node = user.nodes.get(device.nodeId)!;
        let ticket: string = ticketService.createDeviceTicket(user.id, device.id, node.id);

        let token: string = authManager.createSession(user.id);

        let response: LoginDeviceResponse = {
            success: true,
            userId: user.id,
            deviceId: device.id,
            node: node,
            ticket: ticket,
            token: token
        };

        Logger.log('created session [' + response.token + '] for device [' + response.deviceId + ']');

        res.status(200);
        res.json(response);
    } catch(e) {
        res.status(400);
        res.json(errorResponse('login failed', e.getErrorCode()));
    }

    res.end();
});

app.post('/register_device', (req, res) => {

});

app.post('/connect/user', (req, res) => {
    /*if (!req.body.token) {
        res.status(400);
        res.json(errorResponse('bad request', 600));
        res.end();
        return;
    }*/
    
    let nodeId: string | undefined;

    let clientToken: string = authManager.parseAuthHeader(req, AuthType.CLIENT);
    if (!clientToken) {
        res.status(400);
        res.json(errorResponse('bad request', 600));
        res.end();
        return;
    }

    nodeId = authManager.validateNode(clientToken);
    if (!nodeId) {
        res.status(400);
        res.json(errorResponse('unauthorized', 700));
        res.end();
        return;
    }

    try {
        let userId: string = authManager.validateSession(req.body.token);
        let user: User = userManager.getUserInfo(userId);

        let response: ConnectUserResponse = {
            success: true,
            nodes: [],
            devices: Array.from(user.devices.values()),
        };

        for (const [key, value] of user.nodes.entries()) {
            let ticket = ticketService.createUserTicket(user.id, key);
            response.nodes.push({
                node: value,
                ticket: ticket
            });
        }

        Logger.log('user [' + user.id + '] connected');

        res.status(200);
        res.json(response);
    } catch(e) {
        res.status(400);
        res.json(errorResponse('connect failed', e.getErrorCode()));
    }

    res.end();
});

app.post('/reconnect/device', (req, res) => {

});


app.post('/validate_user', (req, res) => {
    if (!req.body.ticket) {
        res.status(400);
        res.json(errorResponse('bad request', 600));
        res.end();
        return;
    }
    
    let nodeId: string;

    let nodeSecret: string = authManager.parseAuthHeader(req, AuthType.NODE);
    if (!nodeSecret) {
        res.status(400);
        res.json(errorResponse('bad request', 600));
        res.end();
        return;
    }

    nodeId = authManager.validateNode(nodeSecret);
    if (!nodeId) {
        res.status(400);
        res.json(errorResponse('unauthorized', 700));
        res.end();
        return;
    }

    try {
        let ticketData: UserTicketData = ticketService.checkUserTicket(req.body.ticket, nodeId);

        let response: ValidateUserResponse = {
            success: true,
            userId: ticketData.userId
        };

        res.status(200)
        res.json(response);
    } catch(e) {
        res.status(400);
        res.json(errorResponse('validate user failed', e.getErrorCode()));
    }

    res.end();
});

app.post('/validate_device', (req, res) => {
    if (!req.body.ticket) {
        res.status(400);
        res.json(errorResponse('bad request', 600));
        res.end();
        return;
    }
    
    let nodeId: string;

    let nodeSecret: string = authManager.parseAuthHeader(req, AuthType.NODE);
    if (!nodeSecret) {
        res.status(400);
        res.json(errorResponse('bad request', 600));
        res.end();
        return;
    }

    nodeId = authManager.validateNode(nodeSecret);
    if (!nodeId) {
        res.status(400);
        res.json(errorResponse('unauthorized', 700));
        res.end();
        return;
    }

    try {
        let ticketData: DeviceTicketData = ticketService.checkDeviceTicket(req.body.ticket, nodeId);

        let response: ValidateDeviceResponse = {
            success: true,
            userId: ticketData.userId,
            deviceId: ticketData.deviceId
        };

        res.status(200)
        res.json(response);
    } catch(e) {
        res.status(400);
        res.json(errorResponse('validate device failed', e.getErrorCode()));
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

Logger.enableDebug();

const httpsServer = https.createServer(credentials, app);
httpsServer.keepAliveTimeout = 30000;

httpsServer.listen(port);

Logger.log('server listening on port ' + port);

