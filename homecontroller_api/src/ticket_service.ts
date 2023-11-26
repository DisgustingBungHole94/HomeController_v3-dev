import { Logger } from './util/logger';
import Exception from './util/exception';
import generateToken from './util/token';

export interface UserTicketData {
    userId: string,
    nodeId: string,
    used: boolean
};

export interface DeviceTicketData {
    userId: string,
    deviceId: string,
    nodeId: string,
    used: boolean
};

export default class TicketService {
    private userTickets: Map<string, UserTicketData>;
    private deviceTickets: Map<string, DeviceTicketData>;

    constructor() {
        this.userTickets = new Map<string, UserTicketData>();
        this.deviceTickets = new Map<string, DeviceTicketData>();
    }

    public createUserTicket(userId: string, nodeId: string): string {
        let ticketData: UserTicketData = {
            userId: userId,
            nodeId: nodeId,
            used: false
        };

        let ticket: string = generateToken(32);
        this.userTickets.set(ticket, ticketData);

        setTimeout((): void => {
            let ticketData = this.userTickets.get(ticket);
            if (ticketData) {
                if (!ticketData.used) {
                    Logger.log('user ticket [' + ticket + '] expired');
                }

                this.userTickets.delete(ticket);
            }
        }, 5000);

        Logger.log('created ticket [' + ticket + '] for user');

        return ticket;
    }


    public checkUserTicket(ticket: string, nodeId: string): UserTicketData {
        if (!this.userTickets.has(ticket)) {
            throw new Exception('bad ticket', 400);
        }

        let ticketData: UserTicketData = this.userTickets.get(ticket)!;

        if (ticketData.used) {
            throw new Exception('ticket used', 400);
        }

        if (ticketData.nodeId !== nodeId) {
            throw new Exception('bad ticket', 400);
        }

        ticketData.used = true;
        this.userTickets.set(ticket, ticketData);

        Logger.log('user [' + ticketData.userId + '] used ticket [' + ticket + ']');

        return ticketData;
    }

    public createDeviceTicket(userId: string, deviceId: string, nodeId: string): string {
        let ticketData: DeviceTicketData = {
            userId: userId,
            deviceId: deviceId,
            nodeId: nodeId,
            used: false
        };

        let ticket: string = generateToken(32);
        this.deviceTickets.set(ticket, ticketData);

        setTimeout((): void => {
            let ticketData = this.deviceTickets.get(ticket);
            if (ticketData) {
                if (!ticketData.used) {
                    Logger.log('device ticket [' + ticket + '] expired');
                }

                this.deviceTickets.delete(ticket);
            }
        }, 5000);

        Logger.log('created ticket [' + ticket + '] for device');

        return ticket;
    }

    public checkDeviceTicket(ticket: string, nodeId: string): DeviceTicketData {        
        if (!this.deviceTickets.has(ticket)) {
            throw new Exception('bad ticket', 400);
        }

        let ticketData: DeviceTicketData = this.deviceTickets.get(ticket)!;

        if (ticketData.used) {
            throw new Exception('ticket used', 400);
        }

        if (ticketData.nodeId !== nodeId) {
            throw new Exception('bad ticket', 400);
        }

        ticketData.used = true;
        this.deviceTickets.set(ticket, ticketData);

        Logger.log('device [' + ticketData.deviceId + '] used ticket [' + ticket + ']');

        return ticketData;
    }
}