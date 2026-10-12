import { useState } from 'react'
import { useNavigate, Link } from 'react-router-dom'
import { useAuth } from '../context/AuthContext'
import StrawberryLogo from '../components/StrawberryLogo'
import { APP_NAME } from '../lib/config'

export default function Login() {
  const { login } = useAuth()
  const nav = useNavigate()
  const [id, setId] = useState('C001')
  const [pin, setPin] = useState('123456')
  const [err, setErr] = useState('')
  const [loading, setLoading] = useState(false)

  const submit = async (e) => {
    e.preventDefault(); setErr(''); setLoading(true)
    try { await login(id, pin); nav('/dashboard') }
    catch (e) { setErr(e.message) }
    finally { setLoading(false) }
  }

  return (
    <div className="page narrow" style={{ paddingTop:'2rem' }}>
      <div style={{ display:'grid', justifyItems:'center', marginBottom:'1.2rem', textAlign:'center' }}>
        <StrawberryLogo size={72} />
        <h1 style={{ fontSize:'1.8rem', marginTop:'0.6rem' }}>{APP_NAME}</h1>
        <p className="muted">Masuk ke akunmu</p>
      </div>
      <form className="card stack" onSubmit={submit}>
        <div className="field">
          <label className="label">ID Nasabah</label>
          <input value={id} onChange={e=>setId(e.target.value)} placeholder="C001" autoFocus />
        </div>
        <div className="field">
          <label className="label">PIN (6 digit)</label>
          <input type="password" value={pin} onChange={e=>setPin(e.target.value)} placeholder="••••••" maxLength={6} />
        </div>
        {err && <div className="tag fail" style={{ justifyContent:'center' }}>{err}</div>}
        <button className="block" disabled={loading}>{loading ? 'Memproses…' : 'Masuk'}</button>
        <div className="tag" style={{ justifyContent:'center' }}>Demo — C001 / 123456 · C002 / 654321</div>
        <p className="muted" style={{ textAlign:'center', fontSize:'0.88rem' }}>
          Belum punya akun? <Link to="/register" style={{ color:'var(--berry-700)', fontWeight:700 }}>Daftar</Link>
        </p>
      </form>
      <div style={{ display:'flex', justifyContent:'center', marginTop:'1rem' }}>
        <Link to="/staff" className="ghost" style={{ fontSize:'0.85rem' }}>
          Login Staff Panel
        </Link>
      </div>
    </div>
  )
}
