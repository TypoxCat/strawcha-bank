// Pewarnaan sintaks C++ ringan, meniru tema VS Code Dark+.
// Tidak memakai dependensi eksternal supaya build tetap offline.

const CONTROL = new Set([
  'if', 'else', 'for', 'while', 'do', 'return', 'throw', 'break',
  'continue', 'switch', 'case', 'default', 'try', 'catch',
])

const STORAGE = new Set([
  'class', 'struct', 'public', 'private', 'protected', 'virtual', 'override',
  'const', 'static', 'void', 'bool', 'int', 'double', 'float', 'char', 'long',
  'short', 'unsigned', 'signed', 'new', 'delete', 'this', 'nullptr', 'true',
  'false', 'auto', 'sizeof', 'using', 'namespace', 'template', 'typename',
  'explicit', 'noexcept', 'friend', 'inline', 'operator', 'std',
  'string', 'size_t', 'vector', 'unique_ptr', 'shared_ptr', 'map', 'pair',
  'optional', 'ostream',
])

const TOKEN = /(\/\/[^\n]*)|("(?:\\.|[^"\\])*")|(#[A-Za-z_]\w*)|(\b\d+(?:\.\d+)?\b)|([A-Za-z_]\w*)|(\s+)|([^\sA-Za-z0-9_]+)/g

export function highlightCpp(code) {
  const out = []
  let m
  let last = 0
  TOKEN.lastIndex = 0
  while ((m = TOKEN.exec(code)) !== null) {
    if (m.index > last) out.push({ text: code.slice(last, m.index) })
    last = TOKEN.lastIndex

    if (m[1]) out.push({ cls: 'cmt', text: m[1] })
    else if (m[2]) out.push({ cls: 'str', text: m[2] })
    else if (m[3]) out.push({ cls: 'pre', text: m[3] })
    else if (m[4]) out.push({ cls: 'num', text: m[4] })
    else if (m[5]) {
      const id = m[5]
      const after = code.slice(last).match(/^\s*(\S)/)
      let cls = 'var'
      if (CONTROL.has(id)) cls = 'kw'
      else if (STORAGE.has(id)) cls = 'type'
      else if (/^[A-Z][A-Z0-9_]+$/.test(id)) cls = 'const'
      else if (after && after[1] === '(') cls = 'fn'
      else if (/^[A-Z]/.test(id)) cls = 'cls'
      out.push({ cls, text: id })
    } else {
      out.push({ text: m[0] })
    }
  }
  if (last < code.length) out.push({ text: code.slice(last) })
  return out
}

export function CodeBlock({ code, className = 'note-code' }) {
  const tokens = highlightCpp(code)
  return (
    <pre className={className}>
      <code>
        {tokens.map((t, i) =>
          t.cls ? <span key={i} className={'tk-' + t.cls}>{t.text}</span> : <span key={i}>{t.text}</span>
        )}
      </code>
    </pre>
  )
}
