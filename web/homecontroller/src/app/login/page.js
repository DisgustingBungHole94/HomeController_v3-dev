'use client';

import { useState } from 'react';
import { useRouter } from 'next/navigation';

import { login } from '../../hc/api_requests.js';

export default function LoginPage() {
    const [username, setUsername] = useState('');
    const [password, setPassword] = useState('');

    const [errorMessage, setErrorMessage] = useState('');
    const [successMessage, setSuccessMessage] = useState('');

    const router = useRouter();
    
    const handleLogin = async (e) => {
        e.preventDefault();

        try {
            const response = await login(username, password);

            setSuccessMessage('Logged in!');

            document.cookie = 'token=' + response.token + '; path=/';
            router.push("/home/devices");
        } catch(err) {
            console.log(err);
            setErrorMessage('Login failed!');
        }
    };
    
    return (
        <div className="bg-gray-700 h-screen flex items-center justify-center">
            <div className="bg-gray-100 py-10 px-10 text-center shadow-xl rounded">
                <div className="my-3">
                    <span className="text-4xl text-gray-700 ">my</span>
                    <span className="text-4xl text-green-500">Home</span>
                    <p className="text-sm text-gray-700 py-1">By Josh Dittmer | v0.0.0</p>
                </div>
                {!successMessage && errorMessage && (
                    <div className="bg-red-300 px-1 py-1 rounded">
                        <span className="text-lg text-red-800">{errorMessage}</span>
                    </div>
                )}
                {successMessage && (
                    <div className="bg-green-300 px-1 py-1 rounded">
                        <span className="text-lg text-green-800">{successMessage}</span>
                    </div>
                )}
                <form onSubmit={handleLogin}>
                    <input 
                        type="text" 
                        value={username} 
                        onChange={(e) => setUsername(e.target.value)} 
                        placeholder="Username..."
                        required 
                        className="border-green-500 border-2 my-4 px-2 py-2 rounded" 
                    />
                    <br />
                    <input 
                        type="password"
                        value={password} 
                        onChange={(e) => setPassword(e.target.value)} 
                        placeholder="Password..." 
                        required 
                        className="border-green-500 border-2 px-2 py-2 rounded" 
                    />
                    <br /><br />
                    <button 
                        type="submit" 
                        className="bg-green-500 hover:bg-green-700 text-white font-bold rounded py-2 px-4"
                    >
                        Login
                    </button>
                </form>
            </div>
        </div>
    );
}