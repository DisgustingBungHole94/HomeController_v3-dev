import { Dispatch, SetStateAction, createContext } from 'react';

export interface ErrorInfo {
    error: string,
    setError: Dispatch<SetStateAction<string>>
};

export const ErrorContext = createContext<ErrorInfo | null>(null);