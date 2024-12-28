import React, { useState, useEffect } from 'react';
import './App.css';
import { ref, set, onValue } from 'firebase/database';
import { auth, database } from './firebase';
import { signInWithEmailAndPassword } from 'firebase/auth';

const App = () => {
  const [orbLit, setOrbLit] = useState(false);
  const [authenticated, setAuthenticated] = useState(false);

  useEffect(() => {
    let retryTimeout;
  
    // Sign in the user with throttling
    const signInUser = () => {
      signInWithEmailAndPassword(auth, "orb1@orb1.com", "password123")
        .then(() => {
          console.log("User authenticated!");
          setAuthenticated(true);
        })
        .catch((error) => {
          console.error("Authentication failed:", error);
          if (error.code === "auth/too-many-requests") {
            console.log("Too many requests. Retrying in 30 seconds...");
            retryTimeout = setTimeout(signInUser, 30000); // Retry after 30 seconds
          }
        });
    };
  
    signInUser();
  
    return () => clearTimeout(retryTimeout); 
  }, []);
  

  useEffect(() => {
    if (authenticated) {
      // Listen for changes to SecondOrbState from the ESP32
      const orbStateRef = ref(database, 'SecondOrbState');
      onValue(orbStateRef, (snapshot) => {
        const value = snapshot.val();
        if (value === 1) {
          setOrbLit(true);
          setTimeout(() => setOrbLit(false), 5000); // Automatically turn off after 5 seconds
        }
      });
    }
  }, [authenticated]);

  const toggleOrb = () => {
    if (authenticated) {
      // Update FirstOrbState in the database to notify the ESP32
      const orbStateRef = ref(database, 'FirstOrbState');
      set(orbStateRef, 1);
      setTimeout(() => set(orbStateRef, 0), 5000); // Reset state after 5 seconds
    } else {
      console.error("User is not authenticated. Cannot toggle orb.");
    }
  };

  return (
    <div className="app-container">
      <div className="orbs-container">
        <div className={`orb ${orbLit ? 'lit' : ''}`}></div>
        <div className="button-container">
          <button className="light-button" onClick={toggleOrb}>
            Light the other orb
          </button>
        </div>
      </div>
    </div>
  );
};

export default App;
