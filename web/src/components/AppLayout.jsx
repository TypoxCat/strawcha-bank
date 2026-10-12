import { Outlet } from 'react-router-dom'
import BottomNav from './BottomNav'
import Footer from './Footer'

export default function AppLayout() {
  return (
    <div className="appframe">
      <div className="app-main">
        <Outlet />
        <Footer />
      </div>
      <BottomNav />
    </div>
  )
}
