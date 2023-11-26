"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const logger_1 = require("./util/logger");
const exception_1 = require("./util/exception");
const token_1 = require("./util/token");
;
;
class TicketService {
    constructor() {
        this.userTickets = new Map();
        this.deviceTickets = new Map();
    }
    createUserTicket(userId, nodeId) {
        let ticketData = {
            userId: userId,
            nodeId: nodeId,
            used: false
        };
        let ticket = (0, token_1.default)(32);
        this.userTickets.set(ticket, ticketData);
        setTimeout(() => {
            let ticketData = this.userTickets.get(ticket);
            if (ticketData) {
                if (!ticketData.used) {
                    logger_1.Logger.log('user ticket [' + ticket + '] expired');
                }
                this.userTickets.delete(ticket);
            }
        }, 5000);
        logger_1.Logger.log('created ticket [' + ticket + '] for user');
        return ticket;
    }
    checkUserTicket(ticket, nodeId) {
        if (!this.userTickets.has(ticket)) {
            throw new exception_1.default('bad ticket', 400);
        }
        let ticketData = this.userTickets.get(ticket);
        if (ticketData.used) {
            throw new exception_1.default('ticket used', 400);
        }
        if (ticketData.nodeId !== nodeId) {
            throw new exception_1.default('bad ticket', 400);
        }
        ticketData.used = true;
        this.userTickets.set(ticket, ticketData);
        logger_1.Logger.log('user [' + ticketData.userId + '] used ticket [' + ticket + ']');
        return ticketData;
    }
    createDeviceTicket(userId, deviceId, nodeId) {
        let ticketData = {
            userId: userId,
            deviceId: deviceId,
            nodeId: nodeId,
            used: false
        };
        let ticket = (0, token_1.default)(32);
        this.deviceTickets.set(ticket, ticketData);
        setTimeout(() => {
            let ticketData = this.deviceTickets.get(ticket);
            if (ticketData) {
                if (!ticketData.used) {
                    logger_1.Logger.log('device ticket [' + ticket + '] expired');
                }
                this.deviceTickets.delete(ticket);
            }
        }, 5000);
        logger_1.Logger.log('created ticket [' + ticket + '] for device');
        return ticket;
    }
    checkDeviceTicket(ticket, nodeId) {
        if (!this.deviceTickets.has(ticket)) {
            throw new exception_1.default('bad ticket', 400);
        }
        let ticketData = this.deviceTickets.get(ticket);
        if (ticketData.used) {
            throw new exception_1.default('ticket used', 400);
        }
        if (ticketData.nodeId !== nodeId) {
            throw new exception_1.default('bad ticket', 400);
        }
        ticketData.used = true;
        this.deviceTickets.set(ticket, ticketData);
        logger_1.Logger.log('device [' + ticketData.deviceId + '] used ticket [' + ticket + ']');
        return ticketData;
    }
}
exports.default = TicketService;
