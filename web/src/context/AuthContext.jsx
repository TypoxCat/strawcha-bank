import React, { createContext, useContext, useEffect, useState } from 'react'
import { api, getToken, setToken } from '../lib/api'

const AuthContext = createContext(null)

export function AuthProvider({ children }) {
  const [user, setUser] = useState(null)
  const [loading, setLoading] = useState(true)

  useEffect(() => {
    async function load() {
      try {
        if (getToken()) {
          const me = await api.me()
          setUser(me)
        }
      } catch (e) {
        setToken(null)
        setUser(null)
      } finally {
        setLoading(false)
      }
    }
    load()
  }, [])

  const login = async (customerId, pin) => {
    const res = await api.login(customerId, pin)
    setToken(res.token)
    setUser(res.customer)
    return res.customer
  }

  const logout = async () => {
    try { await api.logout() } catch (e) {}
    setToken(null)
    setUser(null)
  }

  return (
    <AuthContext.Provider value={{ user, setUser, login, logout, loading }}>
      {children}
    </AuthContext.Provider>
  )
}

export function useAuth() {
  const ctx = useContext(AuthContext)
  if (!ctx) throw new Error('useAuth must be used within AuthProvider')
  return ctx
}