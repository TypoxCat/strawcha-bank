import { useNavigate } from 'react-router-dom'
import { useAuth } from '../context/AuthContext'
import StrawberryLogo from './StrawberryLogo'

export default function AppHeader({ title, subtitle, back = false, greet = false, right }) {
  const nav = useNavigate()
  const { user } = useAuth()

  return (
    <header className="appheader">
      <div className="appheader-inner">
        <div className="appheader-left">
          {back ? (
            <button className="iconbtn" aria-label="Kembali" onClick={() => nav(-1)}>
              <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.2" strokeLinecap="round" strokeLinejoin="round"><path d="M15 5l-7 7 7 7" /></svg>
            </button>
          ) : (
            <StrawberryLogo size={34} />
          )}
          <div>
            {greet ? (
              <>
                <div className="appheader-greet">Halo, {user?.name || 'Nasabah'}</div>
                <div className="appheader-sub">Selamat datang kembali</div>
              </>
            ) : (
              <>
                <div className="appheader-title">{title}</div>
                {subtitle && <div className="appheader-sub">{subtitle}</div>}
              </>
            )}
          </div>
        </div>
        <div className="appheader-right">
          {right}
          {!back && (
            <button className="iconbtn" aria-label="Notifikasi">
              <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"><path d="M18 8a6 6 0 1 0-12 0c0 7-3 8-3 8h18s-3-1-3-8" /><path d="M13.7 21a2 2 0 0 1-3.4 0" /></svg>
            </button>
          )}
          {!back && (
            <button className="avatar" aria-label="Profil" onClick={() => nav('/akun')}>
              <StrawberryLogo size={26} />
            </button>
          )}
        </div>
      </div>
    </header>
  )
}
