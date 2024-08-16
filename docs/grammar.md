$$

\begin{align}
[\text{prog}] &\to [\text{stmt}]^* \\
[\text{stmt}] &\to \begin{cases}
    exit(\text{expr});\\
    var \space \text{ident}=[\text{expr}]
\end{cases}
\\

[\text{expr}] &\to 
\begin{cases}
\text{int\_lit};
\\
\text{ident};
\end{cases}
\\
[\text{BinExpr}] \to 
\begin{cases}
    [\text{expr}] + [\text{expr}] & \text{prec=0}\\
    [\text{expr}] * [\text{expr}] & \text{prec=1}
\end{cases}
\end{align}

$$