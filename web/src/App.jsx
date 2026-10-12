import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom'
import { AuthProvider, useAuth } from './context/AuthContext'
import AppLayout from './components/AppLayout'
import PillarNotes from './components/PillarNotes'
import Login from './pages/Login'
import Register from './pages/Register'
import Dashboard from './pages/Dashboard'
import Accounts from './pages/Accounts'
import Transfer from './pages/Transfer'
import Payments from './pages/Payments'
import History from './pages/History'
import Staff from './pages/Staff'
import Akun from './pages/Akun'
import ChangePin from './pages/ChangePin'

function Protected({ children }) {
  const { user, loading } = useAuth()
  if (loading) return <div className="page">Memuat…</div>
  if (!user) return <Navigate to="/login" replace />
  return children
}

function PublicOnly({ children }) {
  const { user, loading } = useAuth()
  if (loading) return <div className="page">Memuat…</div>
  if (user) return <Navigate to="/dashboard" replace />
  return children
}

export default function App() {
  return (
    <AuthProvider>
      <BrowserRouter>
        <PillarNotes />
        <Routes>
          {/* public */}
          <Route path="/" element={<Navigate to="/login" replace />} />
          <Route path="/login" element={<PublicOnly><Login /></PublicOnly>} />
          <Route path="/register" element={<PublicOnly><Register /></PublicOnly>} />
          <Route path="/staff" element={<Staff />} />

          {/* app (mobile banking shell) */}
          <Route element={<Protected><AppLayout /></Protected>}>
            <Route path="/dashboard" element={<Dashboard />} />
            <Route path="/accounts" element={<Accounts />} />
            <Route path="/accounts/:num" element={<Accounts />} />
            <Route path="/transfer" element={<Transfer />} />
            <Route path="/payments" element={<Payments />} />
            <Route path="/history" element={<History />} />
            <Route path="/akun" element={<Akun />} />
            <Route path="/akun/pin" element={<ChangePin />} />
          </Route>

          <Route path="*" element={<Navigate to="/login" replace />} />
        </Routes>
      </BrowserRouter>
    </AuthProvider>
  )
}
