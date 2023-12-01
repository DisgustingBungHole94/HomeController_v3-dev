import { Dispatch, createContext } from 'react';

export const ErrorContext = createContext<[string, Dispatch<string>] | null>(null);